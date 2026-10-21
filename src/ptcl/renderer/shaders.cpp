#include "ptcl/renderer/shaders.h"

#include <cstddef>
#include <initializer_list>
#include <string_view>


namespace Ptcl::Render::Shaders {


// ========================================================================== //


namespace {

using namespace std::string_view_literals;


std::string compose(std::initializer_list<std::string_view> chunks) {
    std::size_t size = 0;
    for (const std::string_view chunk : chunks) {
        size += chunk.size();
    }

    std::string shader;
    shader.reserve(size);
    for (const std::string_view chunk : chunks) {
        shader.append(chunk);
    }

    return shader;
}


// ========================================================================== //
// Colour combiner
// ========================================================================== //

constexpr std::string_view cCombinerEval = R"(
vec4 stageSrc(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    int inIdx = uCfg[base + 1 + idx];
    if (inIdx == 0) return constant;
    if (inIdx == 1) return primary;
    if (inIdx == 2) return tex;
    return prev;
}

vec3 stageColor(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec4 value = stageSrc(base, idx, constant, primary, tex, prev);
    int op = uCfg[base + 4 + idx];
    if (op == 1) return vec3(value.g);
    if (op == 2) return vec3(1.0) - value.rgb;
    return value.rgb;
}

float stageAlpha(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec4 value = stageSrc(base, idx, constant, primary, tex, prev);
    int op = uCfg[base + 4 + idx];
    if (op == 1) return value.g;
    if (op == 2) return 1.0 - value.a;
    return value.a;
}

vec3 evalColorStage(int base, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec3 a = stageColor(base, 0, constant, primary, tex, prev);
    vec3 b = stageColor(base, 1, constant, primary, tex, prev);
    vec3 c = stageColor(base, 2, constant, primary, tex, prev);
    int fn = uCfg[base];
    vec3 result;
    if (fn == 0) result = a;
    else if (fn == 1) result = a * b;
    else if (fn == 2) result = max(a - b, vec3(0.0));
    else result = clamp(a * b + c, vec3(0.0), vec3(1.0));
    if (uCfg[base + 7] == 1) result = min(result * 2.0, vec3(1.0));
    return result;
}

float evalAlphaStage(int base, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    float a = stageAlpha(base, 0, constant, primary, tex, prev);
    float b = stageAlpha(base, 1, constant, primary, tex, prev);
    float c = stageAlpha(base, 2, constant, primary, tex, prev);
    int fn = uCfg[base];
    float result;
    if (fn == 0) result = a;
    else if (fn == 1) result = a * b;
    else if (fn == 2) result = max(a - b, 0.0);
    else result = clamp(a * b + c, 0.0, 1.0);
    if (uCfg[base + 7] == 1) result = min(result * 2.0, 1.0);
    return result;
}

vec4 combine(vec4 tex) {
    vec4 primary = vec4(vColor.rgb, vParticleAlpha);

    vec4 prev = vec4(0.0);
    vec3 color = evalColorStage(0, uConstantColor, primary, tex, prev);
    prev = vec4(color, 0.0);
    color = evalColorStage(8, uConstantColor, primary, tex, prev);

    prev = vec4(0.0);
    float alpha = evalAlphaStage(16, uConstantColor, primary, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, alpha);
    alpha = evalAlphaStage(24, uConstantColor, primary, tex, prev);

    return vec4(color, alpha);
}
)"sv;


// ========================================================================== //
// Quad / stripe fragment shader
// ========================================================================== //

constexpr std::string_view cQuadFragHeader = R"(
#version 150 core
in vec2 vUv;
in vec2 vLocalUv;
in vec4 vColor;
in float vParticleAlpha;
uniform sampler2D uTex;
uniform int uHasTex;
uniform vec4 uConstantColor;
uniform int uCfg[32];
uniform float uAlphaRef;
out vec4 fragColor;
)"sv;

constexpr std::string_view cQuadFragMain = R"(
void main() {
    vec4 tex;
    if (uHasTex == 1) {
        tex = texture(uTex, vUv);
    } else {
        vec2 offset = vLocalUv - vec2(0.5);
        tex = vec4(vec3(1.0), step(length(offset), 0.5));
    }
    fragColor = combine(tex);
    // PICA alpha test: GREATER against the reference the record selects.
    if (fragColor.a <= uAlphaRef) {
        discard;
    }
}
)"sv;


// ========================================================================== //
// Compact fragment shader
// ========================================================================== //

constexpr std::string_view cCompactFragHeader = R"(
#version 150 core
in vec2 vUv;
in vec2 vUvScale;
in vec4 vColor;
in float vParticleAlpha;
in float vRotation;
uniform sampler2D uTex;
uniform int uHasTex;
uniform int uWrapS;
uniform int uWrapT;
uniform vec2 uTexel;
uniform vec4 uConstantColor;
uniform int uCfg[32];
uniform float uAlphaRef;
out vec4 fragColor;
)"sv;

constexpr std::string_view cCompactFragMain = R"(
vec2 pointLocal(vec2 pointCoord, float rotation) {
    float sinRotation = sin(-rotation);
    float cosRotation = cos(-rotation);
    vec2 local = pointCoord - vec2(0.5);
    return vec2(
        local.x * cosRotation - local.y * sinRotation,
        local.x * sinRotation + local.y * cosRotation) + vec2(0.5);
}

float wrapCoordinate(float value, int mode) {
    if (mode == 1) {
        return fract(value);
    }
    if (mode == 0) {
        return abs(fract(value * 0.5) * 2.0 - 1.0);
    }
    return clamp(value, 0.0, 1.0);
}

void main() {
    vec2 local = pointLocal(gl_PointCoord, vRotation);

    vec4 tex;
    if (uHasTex == 1) {
        vec2 localUv = vec2(local.x, 1.0 - local.y);
        vec2 uv = mix(vUv + uTexel * 0.5, vUv + vUvScale - uTexel * 0.5, localUv);
        uv = vec2(wrapCoordinate(uv.x, uWrapS), wrapCoordinate(uv.y, uWrapT));
        tex = texture(uTex, uv);
    } else {
        tex = vec4(1.0, 1.0, 1.0, step(length(local), 0.5));
    }

    fragColor = combine(tex);
    // PICA alpha test: GREATER against the reference the record selects.
    if (fragColor.a <= uAlphaRef) {
        discard;
    }
}
)"sv;


// ========================================================================== //
// Vertex shaders
// ========================================================================== //

constexpr std::string_view cParticleVert = R"(#version 150 core
in vec2 aPos;
uniform vec3 uCenter;
uniform vec3 uAxisX;
uniform vec3 uAxisY;
uniform mat4 uViewProj;
uniform vec2 uUvOffset;
uniform vec2 uUvScale;
uniform vec2 uTexel;
uniform vec4 uPrimaryColor;
out vec2 vUv;
out vec2 vLocalUv;
out vec4 vColor;
out float vParticleAlpha;
void main() {
    vLocalUv = vec2(aPos.x + 0.5, 0.5 - aPos.y);
    vColor = uPrimaryColor;
    vParticleAlpha = uPrimaryColor.a;
    vec2 uvMin = uUvOffset + uTexel * 0.5;
    vec2 uvMax = uUvOffset + uUvScale - uTexel * 0.5;
    vUv = mix(uvMin, uvMax, vLocalUv);
    vec3 world = uCenter + aPos.x * uAxisX + aPos.y * uAxisY;
    gl_Position = uViewProj * vec4(world, 1.0);
}
)"sv;

constexpr std::string_view cStripeVert = R"(#version 150 core
in vec3 aPosition;
in vec2 aUv;
in float aAlpha;
uniform mat4 uViewProj;
uniform vec4 uPrimaryColor;
out vec2 vUv;
out vec2 vLocalUv;
out vec4 vColor;
out float vParticleAlpha;
void main() {
    vUv = vec2(aUv.x, 1.0 - aUv.y);
    vLocalUv = vec2(0.5);
    vColor = vec4(uPrimaryColor.rgb, aAlpha);
    vParticleAlpha = aAlpha;
    gl_Position = uViewProj * vec4(aPosition, 1.0);
}
)"sv;

constexpr std::string_view cCompactVert = R"(#version 150 core
in vec4 aCenterRotation;
in vec4 aScale;
in vec4 aColor;
in vec4 aUv;
uniform mat4 uViewProj;
uniform float uPointScale;
out vec2 vUv;
out vec2 vUvScale;
out vec4 vColor;
out float vParticleAlpha;
out float vRotation;

void main() {
    vUv = aUv.xy;
    vUvScale = aUv.zw;
    vColor = aColor;
    vParticleAlpha = aColor.a;
    vRotation = radians(aCenterRotation.w);
    gl_PointSize = max(1.0, max(aScale.x, aScale.y) * 2.0 * uPointScale);
    gl_Position = uViewProj * vec4(aCenterRotation.xyz, 1.0);
}
)"sv;


} // namespace


// ========================================================================== //


const std::string& particleVertex() {
    static const std::string shader{cParticleVert};
    return shader;
}

const std::string& particleFragment() {
    static const std::string shader = compose({cQuadFragHeader, cCombinerEval, cQuadFragMain});
    return shader;
}

const std::string& stripeVertex() {
    static const std::string shader{cStripeVert};
    return shader;
}

const std::string& compactVertex() {
    static const std::string shader{cCompactVert};
    return shader;
}

const std::string& compactFragment() {
    static const std::string shader = compose({cCompactFragHeader, cCombinerEval, cCompactFragMain});
    return shader;
}


// ========================================================================== //


} // namespace Ptcl::Render::Shaders
