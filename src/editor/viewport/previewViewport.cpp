#include "editor/viewport/previewViewport.h"

#include "editor/components/combinerPreviewWidget.h"
#include "ptcl/ptclDocument.h"
#include "ptcl/ptclEmitterSet.h"
#include "ptcl/ptclTexture.h"

#include <QSurfaceFormat>
#include <QWheelEvent>

#include <array>
#include <cstddef>


namespace PtclTool {


namespace {


constexpr const char* cVertShader = R"(#version 330 core
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
out float vParticleAlpha;
void main() {
    vLocalUv = vec2(aPos.x + 0.5, 0.5 - aPos.y);
    vParticleAlpha = uPrimaryColor.a;
    vec2 uvMin = uUvOffset + uTexel * 0.5;
    vec2 uvMax = uUvOffset + uUvScale - uTexel * 0.5;
    vUv = mix(uvMin, uvMax, vLocalUv);
    vec3 world = uCenter + aPos.x * uAxisX + aPos.y * uAxisY;
    gl_Position = uViewProj * vec4(world, 1.0);
}
)";

constexpr const char* cStripeVertShader = R"(#version 330 core
in vec3 aPosition;
in vec2 aUv;
in float aAlpha;
uniform mat4 uViewProj;
out vec2 vUv;
out vec2 vLocalUv;
out float vParticleAlpha;
void main() {
    vUv = vec2(aUv.x, 1.0 - aUv.y);
    vLocalUv = vec2(0.5);
    vParticleAlpha = aAlpha;
    gl_Position = uViewProj * vec4(aPosition, 1.0);
}
)";

constexpr const char* cFragShader = R"(#version 330 core
in vec2 vUv;
in vec2 vLocalUv;
in float vParticleAlpha;
uniform sampler2D uTex;
uniform int uHasTex;
uniform vec4 uConstantColor;
uniform vec4 uPrimaryColor;
uniform int uCfg[32];
out vec4 fragColor;

vec4 stageSrc(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    int inIdx = uCfg[base + 1 + idx];
    if (inIdx == 0) return constant;
    else if (inIdx == 1) return primary;
    else if (inIdx == 2) return tex;
    else return prev;
}

vec3 stageColor(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec4 v = stageSrc(base, idx, constant, primary, tex, prev);
    int op = uCfg[base + 4 + idx];
    if (op == 1) return vec3(v.g);
    else if (op == 2) return vec3(1.0) - v.rgb;
    else return v.rgb;
}

float stageAlpha(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec4 v = stageSrc(base, idx, constant, primary, tex, prev);
    int op = uCfg[base + 4 + idx];
    if (op == 1) return v.g;
    else if (op == 2) return 1.0 - v.a;
    else return v.a;
}

vec3 evalColorStage(int base, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    int fn = uCfg[base];
    vec3 a = stageColor(base, 0, constant, primary, tex, prev);
    vec3 b = stageColor(base, 1, constant, primary, tex, prev);
    vec3 c = stageColor(base, 2, constant, primary, tex, prev);
    vec3 outColor;
    if (fn == 0) outColor = a;
    else if (fn == 1) outColor = a * b;
    else if (fn == 2) outColor = max(a - b, vec3(0.0));
    else outColor = clamp(a * b + c, vec3(0.0), vec3(1.0));
    if (uCfg[base + 7] == 1) outColor = min(outColor * 2.0, vec3(1.0));
    return outColor;
}

float evalAlphaStage(int base, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    int fn = uCfg[base];
    float a = stageAlpha(base, 0, constant, primary, tex, prev);
    float b = stageAlpha(base, 1, constant, primary, tex, prev);
    float c = stageAlpha(base, 2, constant, primary, tex, prev);
    float outVal;
    if (fn == 0) outVal = a;
    else if (fn == 1) outVal = a * b;
    else if (fn == 2) outVal = max(a - b, 0.0);
    else outVal = clamp(a * b + c, 0.0, 1.0);
    if (uCfg[base + 7] == 1) outVal = min(outVal * 2.0, 1.0);
    return outVal;
}

void main() {
    vec4 tex;
    if (uHasTex == 1) {
        tex = texture(uTex, vUv);
    } else {
        vec2 c = vLocalUv - vec2(0.5);
        float m = step(length(c), 0.5);
        tex = vec4(vec3(1.0), m);
    }
    vec4 primary = vec4(uPrimaryColor.rgb, vParticleAlpha);
    vec4 prev = vec4(0.0);
    vec3 cOut = evalColorStage(0, uConstantColor, primary, tex, prev);
    prev = vec4(cOut, 0.0);
    cOut = evalColorStage(8, uConstantColor, primary, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, 0.0);
    float aOut = evalAlphaStage(16, uConstantColor, primary, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, aOut);
    aOut = evalAlphaStage(24, uConstantColor, primary, tex, prev);
    fragColor = vec4(cOut, aOut);
}
)";

constexpr const char* cVertShaderEs = R"(#version 300 es
precision highp float;
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
out float vParticleAlpha;
void main() {
    vLocalUv = vec2(aPos.x + 0.5, 0.5 - aPos.y);
    vParticleAlpha = uPrimaryColor.a;
    vec2 uvMin = uUvOffset + uTexel * 0.5;
    vec2 uvMax = uUvOffset + uUvScale - uTexel * 0.5;
    vUv = mix(uvMin, uvMax, vLocalUv);
    vec3 world = uCenter + aPos.x * uAxisX + aPos.y * uAxisY;
    gl_Position = uViewProj * vec4(world, 1.0);
}
)";

constexpr const char* cStripeVertShaderEs = R"(#version 300 es
precision highp float;
in vec3 aPosition;
in vec2 aUv;
in float aAlpha;
uniform mat4 uViewProj;
out vec2 vUv;
out vec2 vLocalUv;
out float vParticleAlpha;
void main() {
    vUv = vec2(aUv.x, 1.0 - aUv.y);
    vLocalUv = vec2(0.5);
    vParticleAlpha = aAlpha;
    gl_Position = uViewProj * vec4(aPosition, 1.0);
}
)";

constexpr const char* cFragShaderEs = R"(#version 300 es
precision highp float;
in vec2 vUv;
in vec2 vLocalUv;
in float vParticleAlpha;
uniform sampler2D uTex;
uniform int uHasTex;
uniform vec4 uConstantColor;
uniform vec4 uPrimaryColor;
uniform int uCfg[32];
out vec4 fragColor;

vec4 stageSrc(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    int inIdx = uCfg[base + 1 + idx];
    if (inIdx == 0) return constant;
    else if (inIdx == 1) return primary;
    else if (inIdx == 2) return tex;
    else return prev;
}

vec3 stageColor(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec4 v = stageSrc(base, idx, constant, primary, tex, prev);
    int op = uCfg[base + 4 + idx];
    if (op == 1) return vec3(v.g);
    else if (op == 2) return vec3(1.0) - v.rgb;
    else return v.rgb;
}

float stageAlpha(int base, int idx, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    vec4 v = stageSrc(base, idx, constant, primary, tex, prev);
    int op = uCfg[base + 4 + idx];
    if (op == 1) return v.g;
    else if (op == 2) return 1.0 - v.a;
    else return v.a;
}

vec3 evalColorStage(int base, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    int fn = uCfg[base];
    vec3 a = stageColor(base, 0, constant, primary, tex, prev);
    vec3 b = stageColor(base, 1, constant, primary, tex, prev);
    vec3 c = stageColor(base, 2, constant, primary, tex, prev);
    vec3 outColor;
    if (fn == 0) outColor = a;
    else if (fn == 1) outColor = a * b;
    else if (fn == 2) outColor = max(a - b, vec3(0.0));
    else outColor = clamp(a * b + c, vec3(0.0), vec3(1.0));
    if (uCfg[base + 7] == 1) outColor = min(outColor * 2.0, vec3(1.0));
    return outColor;
}

float evalAlphaStage(int base, vec4 constant, vec4 primary, vec4 tex, vec4 prev) {
    int fn = uCfg[base];
    float a = stageAlpha(base, 0, constant, primary, tex, prev);
    float b = stageAlpha(base, 1, constant, primary, tex, prev);
    float c = stageAlpha(base, 2, constant, primary, tex, prev);
    float outVal;
    if (fn == 0) outVal = a;
    else if (fn == 1) outVal = a * b;
    else if (fn == 2) outVal = max(a - b, 0.0);
    else outVal = clamp(a * b + c, 0.0, 1.0);
    if (uCfg[base + 7] == 1) outVal = min(outVal * 2.0, 1.0);
    return outVal;
}

void main() {
    vec4 tex;
    if (uHasTex == 1) {
        tex = texture(uTex, vUv);
    } else {
        vec2 c = vLocalUv - vec2(0.5);
        float m = step(length(c), 0.5);
        tex = vec4(vec3(1.0), m);
    }
    vec4 primary = vec4(uPrimaryColor.rgb, vParticleAlpha);
    vec4 prev = vec4(0.0);
    vec3 cOut = evalColorStage(0, uConstantColor, primary, tex, prev);
    prev = vec4(cOut, 0.0);
    cOut = evalColorStage(8, uConstantColor, primary, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, 0.0);
    float aOut = evalAlphaStage(16, uConstantColor, primary, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, aOut);
    aOut = evalAlphaStage(24, uConstantColor, primary, tex, prev);
    fragColor = vec4(cOut, aOut);
}
)";


constexpr const char* cScreenVertShader = R"(#version 330 core
in vec2 aPos;
void main() {
    gl_Position = vec4(aPos * 2.0, 0.0, 1.0);
}
)";

constexpr const char* cScreenFragShader = R"(#version 330 core
out vec4 fragColor;
void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)";

constexpr const char* cScreenVertShaderEs = R"(#version 300 es
precision highp float;
in vec2 aPos;
void main() {
    gl_Position = vec4(aPos * 2.0, 0.0, 1.0);
}
)";

constexpr const char* cScreenFragShaderEs = R"(#version 300 es
precision highp float;
out vec4 fragColor;
void main() {
    fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}
)";

constexpr const char* cGridVertShader = R"(#version 330 core
in vec3 aPos;
uniform mat4 uViewProj;
void main() {
    gl_Position = uViewProj * vec4(aPos, 1.0);
}
)";

constexpr const char* cGridFragShader = R"(#version 330 core
uniform vec4 uColor;
out vec4 fragColor;
void main() {
    fragColor = uColor;
}
)";

constexpr const char* cGridVertShaderEs = R"(#version 300 es
precision highp float;
in vec3 aPos;
uniform mat4 uViewProj;
void main() {
    gl_Position = uViewProj * vec4(aPos, 1.0);
}
)";

constexpr const char* cGridFragShaderEs = R"(#version 300 es
precision highp float;
uniform vec4 uColor;
out vec4 fragColor;
void main() {
    fragColor = uColor;
}
)";


std::array<GLint, 32> buildCombinerCfg(Ptcl::ColorCombinerFuncType func) {
    std::array<GLint, 32> cfgData{};
    const auto idx = static_cast<std::size_t>(func);
    if (idx >= Combiner::configurations.size()) {
        return cfgData;
    }

    const Combiner::Config& config = Combiner::configurations[idx];
    const Combiner::Stage* stages[4] = {
        &config.colorStage[0], &config.colorStage[1],
        &config.alphaStage[0], &config.alphaStage[1],
    };

    for (s32 s = 0; s < 4; ++s) {
        const Combiner::Stage& stage = *stages[s];
        const s32 base = s * 8;
        cfgData[base + 0] = static_cast<GLint>(stage.func);
        cfgData[base + 1] = static_cast<GLint>(stage.inputs[0]);
        cfgData[base + 2] = static_cast<GLint>(stage.inputs[1]);
        cfgData[base + 3] = static_cast<GLint>(stage.inputs[2]);
        cfgData[base + 4] = static_cast<GLint>(stage.operands[0]);
        cfgData[base + 5] = static_cast<GLint>(stage.operands[1]);
        cfgData[base + 6] = static_cast<GLint>(stage.operands[2]);
        cfgData[base + 7] = static_cast<GLint>(stage.scale);
    }
    return cfgData;
}

GLuint uploadTexture(const Ptcl::Texture& texture) {
    QImage img = texture.textureData();
    if (img.isNull()) {
        return 0;
    }
    img = img.convertToFormat(QImage::Format_RGBA8888);

    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, img.width(), img.height(), 0, GL_RGBA,
        GL_UNSIGNED_BYTE, img.constBits());
    return id;
}

void setupRenderState(QOpenGLFunctions_2_0& gl, Ptcl::BlendFuncType blend, Ptcl::DepthFuncType depth) {
    switch (blend) {
    case Ptcl::BlendFuncType::AlphaBlend:
        gl.glBlendEquation(GL_FUNC_ADD);
        gl.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
        break;
    case Ptcl::BlendFuncType::Additive:
        gl.glBlendEquation(GL_FUNC_ADD);
        gl.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
        break;
    case Ptcl::BlendFuncType::Subtractive:
        gl.glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
        gl.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
        break;
    case Ptcl::BlendFuncType::Opaque:
        gl.glBlendEquation(GL_FUNC_ADD);
        gl.glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
        break;
    default:
        break;
    }

    switch (depth) {
    case Ptcl::DepthFuncType::TestOnly:
        gl.glEnable(GL_DEPTH_TEST);
        gl.glDepthFunc(GL_LESS);
        gl.glDepthMask(GL_FALSE);
        gl.glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
        break;
    case Ptcl::DepthFuncType::Disabled:
        gl.glDisable(GL_DEPTH_TEST);
        gl.glDepthMask(GL_FALSE);
        gl.glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        break;
    case Ptcl::DepthFuncType::TestAndWrite:
        gl.glEnable(GL_DEPTH_TEST);
        gl.glDepthFunc(GL_LESS);
        gl.glDepthMask(GL_TRUE);
        gl.glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        break;
    default:
        break;
    }
}

using Mat4 = std::array<GLfloat, 16>;

struct StripeGLVertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat u;
    GLfloat v;
    GLfloat alpha;
};

Mat4 multiplyMat4(const Mat4& a, const Mat4& b) {
    Mat4 out{};
    for (int col = 0; col < 4; ++col) {
        for (int row = 0; row < 4; ++row) {
            GLfloat sum = 0.0f;
            for (int k = 0; k < 4; ++k) {
                sum += b[col * 4 + k] * a[k * 4 + row];
            }
            out[col * 4 + row] = sum;
        }
    }
    return out;
}

Mat4 makePerspective(float fovYRadians, float aspect, float nearZ, float farZ) {
    const float focal = 1.0f / std::tan(fovYRadians * 0.5f);
    Mat4 m{};
    m[0] = focal / aspect;
    m[5] = focal;
    m[10] = (farZ + nearZ) / (nearZ - farZ);
    m[11] = -1.0f;
    m[14] = (2.0f * farZ * nearZ) / (nearZ - farZ);
    return m;
}

Mat4 makeLookAt(float ex, float ey, float ez, float cx, float cy, float cz, float upx, float upy, float upz) {
    float fx = cx - ex;
    float fy = cy - ey;
    float fz = cz - ez;
    float fl = std::sqrt(fx * fx + fy * fy + fz * fz);
    if (fl > 0.0f) {
        fx /= fl;
        fy /= fl;
        fz /= fl;
    }

    float sx = fy * upz - fz * upy;
    float sy = fz * upx - fx * upz;
    float sz = fx * upy - fy * upx;
    float sl = std::sqrt(sx * sx + sy * sy + sz * sz);
    if (sl > 0.0f) {
        sx /= sl;
        sy /= sl;
        sz /= sl;
    }

    float ux = sy * fz - sz * fy;
    float uy = sz * fx - sx * fz;
    float uz = sx * fy - sy * fx;

    Mat4 m{};
    m[0] = sx;
    m[1] = ux;
    m[2] = -fx;
    m[4] = sy;
    m[5] = uy;
    m[6] = -fy;
    m[8] = sz;
    m[9] = uz;
    m[10] = -fz;
    m[12] = -(sx * ex + sy * ey + sz * ez);
    m[13] = -(ux * ex + uy * ey + uz * ez);
    m[14] = fx * ex + fy * ey + fz * ez;
    m[15] = 1.0f;
    return m;
}

constexpr float cCameraDistance = 400.0f;
constexpr float cVerticalFov = 0.7853981633974483f; // 45 degrees.
constexpr float cNear = 1.0f;
constexpr float cFar = 2000.0f;
constexpr float cZoomMin = 0.1f;
constexpr float cZoomMax = 10.0f;
constexpr float cZoomStep = 0.1f;
constexpr float cZoomExponent = 1.1f;

struct CameraBasis {
    Math::Vector3f right;
    Math::Vector3f up;
    Math::Vector3f front;
};

CameraBasis makeCameraBasis(float yaw, float pitch, const Math::Vector3f& target) {
    const float cyaw = std::cos(yaw);
    const float syaw = std::sin(yaw);
    const float cpitch = std::cos(pitch);
    const float spitch = std::sin(pitch);

    const float ex = target.getX() + cCameraDistance * cpitch * syaw;
    const float ey = target.getY() + cCameraDistance * spitch;
    const float ez = target.getZ() + cCameraDistance * cpitch * cyaw;

    const Mat4 view = makeLookAt(ex, ey, ez, target.getX(), target.getY(), target.getZ(), 0.0f, 1.0f, 0.0f);

    CameraBasis basis;
    basis.right = {view[0], view[4], view[8]};
    basis.up = {view[1], view[5], view[9]};
    basis.front = {-view[2], -view[6], -view[10]};
    return basis;
}

void computeParticleAxes(const Ptcl::Engine::EmitterState& state,
    const Ptcl::Engine::ParticleState& particle, const CameraBasis& cam,
    Math::Vector3f& axisX, Math::Vector3f& axisY) {
    const f32 width = particle.scale.getX();
    const f32 height = particle.scale.getY();
    const auto& m = particle.matrix;

    const auto& followBasis = particle.followMode == Ptcl::Engine::ParticleFollowMode::Basis ? particle.followBasis : state.emitterRT;
    const auto localX = Math::Vector3f{m(0, 3), m(1, 3), m(2, 3)};
    const auto localY = Math::Vector3f{m(0, 2), m(1, 2), m(2, 2)};
    const auto localZ = Math::Vector3f{m(0, 1), m(1, 1), m(2, 1)};
    const auto worldX = Math::VectorCalc::rotate(followBasis, localX);
    const auto worldY = Math::VectorCalc::rotate(followBasis, localY);

    switch (state.billboardType) {
    case Ptcl::BillboardType::BillboardY: {
        const Math::Vector3f worldUp{0.0f, 1.0f, 0.0f};
        axisX = Math::VectorCalc::add(
            Math::VectorCalc::multScalar(cam.right, localX.getX()),
            Math::VectorCalc::multScalar(cam.up, localX.getY()));
        Math::VectorCalc::normalize(axisX);
        axisX = Math::VectorCalc::multScalar(axisX, width);
        axisY = Math::VectorCalc::multScalar(worldUp, height);
        break;
    }

    case Ptcl::BillboardType::PolygonXY:
        axisX = Math::VectorCalc::multScalar(worldX, width);
        axisY = Math::VectorCalc::multScalar(worldY, height);
        break;

    case Ptcl::BillboardType::PolygonXZ:
        axisX = Math::VectorCalc::multScalar(worldX, width);
        axisY = Math::VectorCalc::multScalar(
            Math::VectorCalc::rotate(followBasis, Math::Vector3f{m(0, 1), m(1, 1), m(2, 1)}), height);
        break;

    case Ptcl::BillboardType::VelLook:
    case Ptcl::BillboardType::VelLookPolygon: {
        Math::Vector3f dir = particle.followDirection;
        if (dir.length() <= 0.0001f) {
            dir = cam.up;
        }
        Math::VectorCalc::normalize(dir);


        const f32 dirDotLocalY = dir.getX() * localY.getX()
            + dir.getY() * localY.getY() + dir.getZ() * localY.getZ();
        const f32 longitudinalSign = (!particle.isChild && dirDotLocalY < 0.0f)
            ? -1.0f : 1.0f;
        axisY = Math::VectorCalc::multScalar(dir, height * longitudinalSign);
        Math::Vector3f baseRight;
        if (state.billboardType == Ptcl::BillboardType::VelLookPolygon) {
            const Math::Vector3f emitterRight =
                Math::VectorCalc::rotate(followBasis, {1.0f, 0.0f, 0.0f});
            baseRight = Math::VectorCalc::cross(dir, emitterRight);
        } else {
            baseRight = Math::VectorCalc::cross(dir, cam.front);
        }
        if (baseRight.length() <= 0.0001f) {
            baseRight = cam.right;
        }
        if (longitudinalSign < 0.0f) {
            baseRight = Math::VectorCalc::multScalar(baseRight, -1.0f);
        }
        Math::VectorCalc::normalize(baseRight);

        const f32 dirDotReferenceX = dir.getX();
        const Math::Vector3f referenceRight{
            1.0f - dir.getX() * dirDotReferenceX,
            -dir.getY() * dirDotReferenceX,
            -dir.getZ() * dirDotReferenceX,
        };
        const f32 dirDotLocalX = dir.getX() * localX.getX()
            + dir.getY() * localX.getY() + dir.getZ() * localX.getZ();
        const Math::Vector3f rotatedReferenceRight{
            localX.getX() - dir.getX() * dirDotLocalX,
            localX.getY() - dir.getY() * dirDotLocalX,
            localX.getZ() - dir.getZ() * dirDotLocalX,
        };
        Math::Vector3f right = baseRight;
        if (referenceRight.length() > 0.0001f && rotatedReferenceRight.length() > 0.0001f) {
            const Math::Vector3f ref = Math::VectorCalc::multScalar(
                referenceRight, 1.0f / referenceRight.length());
            const Math::Vector3f rotated = Math::VectorCalc::multScalar(
                rotatedReferenceRight, 1.0f / rotatedReferenceRight.length());
            const f32 cosine = ref.getX() * rotated.getX()
                + ref.getY() * rotated.getY() + ref.getZ() * rotated.getZ();
            const Math::Vector3f sineVector = Math::VectorCalc::cross(ref, rotated);
            const f32 sine = sineVector.getX() * dir.getX()
                + sineVector.getY() * dir.getY() + sineVector.getZ() * dir.getZ();
            const Math::Vector3f tangent = Math::VectorCalc::cross(dir, baseRight);
            right = Math::VectorCalc::add(
                Math::VectorCalc::multScalar(baseRight, cosine),
                Math::VectorCalc::multScalar(tangent, sine));
            Math::VectorCalc::normalize(right);
        }
        axisX = Math::VectorCalc::multScalar(right, width);
        break;
    }

    case Ptcl::BillboardType::Stripe:
    case Ptcl::BillboardType::ComplexStripe:
    case Ptcl::BillboardType::Primitive:
    default:
        axisX = Math::VectorCalc::multScalar(worldX, width);
        axisY = Math::VectorCalc::multScalar(worldY, height);
        break;

    case Ptcl::BillboardType::Billboard: {
        if (state.isEmitterBillboardMtx) {
            axisX = Math::VectorCalc::multScalar(worldX, width);
            axisY = Math::VectorCalc::multScalar(worldY, height);
            break;
        }

        const auto transformCameraAxis = [&](const Math::Vector3f& cameraAxis) {
            return Math::VectorCalc::add(
                Math::VectorCalc::add(
                    Math::VectorCalc::multScalar(localX, cameraAxis.getX()),
                    Math::VectorCalc::multScalar(localY, cameraAxis.getY())),
                Math::VectorCalc::multScalar(localZ, cameraAxis.getZ()));
        };
        const Math::Vector3f cameraX = transformCameraAxis(cam.right);
        const Math::Vector3f cameraY = transformCameraAxis(cam.up);
        axisX = Math::VectorCalc::multScalar(cameraX, width);
        axisY = Math::VectorCalc::multScalar(cameraY, height);
        break;
    }
    }
}

Mat4 buildViewProj(float yaw, float pitch, const Math::Vector3f& target,
    float zoomFactor, int viewportWidth, int viewportHeight) {
    const float cyaw = std::cos(yaw);
    const float syaw = std::sin(yaw);
    const float cpitch = std::cos(pitch);
    const float spitch = std::sin(pitch);

    const float ex = target.getX() + cCameraDistance * cpitch * syaw;
    const float ey = target.getY() + cCameraDistance * spitch;
    const float ez = target.getZ() + cCameraDistance * cpitch * cyaw;

    const Mat4 view = makeLookAt(ex, ey, ez, target.getX(), target.getY(), target.getZ(), 0.0f, 1.0f, 0.0f);

    const float aspect = (viewportHeight > 0)
        ? static_cast<float>(viewportWidth) / static_cast<float>(viewportHeight) : 1.0f;
    Mat4 proj = makePerspective(cVerticalFov, aspect, cNear, cFar);
    proj[5] *= zoomFactor;

    return multiplyMat4(proj, view);
}


} // namespace


// ========================================================================== //


PreviewViewport::PreviewViewport(QWidget* parent) :
    QOpenGLWidget{parent} {
    QSurfaceFormat fmt = format();
    fmt.setDepthBufferSize(24);
    setFormat(fmt);

    mSystem = std::make_unique<Ptcl::Engine::PtclSystem>(8);

    setMinimumSize(320, 240);
    mTimer.setTimerType(Qt::PreciseTimer);
    mTimer.setInterval(1);
    connect(&mTimer, &QTimer::timeout, this, &PreviewViewport::stepFrame);
}

PreviewViewport::~PreviewViewport() {
    mTimer.stop();
    disconnect(context(), nullptr, this, nullptr);
}

void PreviewViewport::setPreview(const Ptcl::Document* document, const Ptcl::Selection& selection) {
    if (document == nullptr || selection.type() == Ptcl::Selection::Type::None) {
        clearPreview();
        return;
    }

    const s32 setIndex = selection.emitterSetIndex();
    const Ptcl::EmitterSet* set = (setIndex >= 0) ? document->emitterSet(setIndex) : nullptr;
    if (set == nullptr) {
        clearPreview();
        return;
    }

    mEmitterSet = set;
    createEmitterSet(*set);
    mTimer.start();
    update();
}

void PreviewViewport::clearPreview() {
    mTimer.stop();
    mEmitterSet = nullptr;
    mEmitters.clear();
    mSetInstance = {};
    mSystem->reset();
    mLastDrawCount = 0;
    update();
}

void PreviewViewport::createEmitterSet(const Ptcl::EmitterSet& emitterSet) {
    mEmitters.clear();
    mSetInstance = {};
    mSystem->reset();

    u32 setSeed = 0;
    bool hasSetSeed = false;
    for (const auto& res : emitterSet.emitters()) {
        const auto mode = res->randomSeed().mode();
        u32 seed = 0;
        if (mode == PtclSeed::Mode::ConstantSeed) {
            seed = res->randomSeed().constantSeed();
        } else if (mode == PtclSeed::Mode::RandomPerSet) {
            if (!hasSetSeed) {
                setSeed = mSystem->nextEmitterSeed();
                hasSetSeed = true;
            }
            seed = setSeed;
        } else {
            seed = mSystem->nextEmitterSeed();
        }

        const u32 vec3Index = seed & 0xFFFFu;
        const u32 normalizedVec3Index = (mode == PtclSeed::Mode::ConstantSeed)
            ? 0u
            : (seed >> 16) & 0xFFFFu;
        std::unique_ptr<Ptcl::Engine::EmitterInstance> instance =
            std::make_unique<Ptcl::Engine::EmitterInstance>(*res, mSetInstance, seed, vec3Index, normalizedVec3Index);

        mSetInstance.addEmitter(*instance);
        mSystem->addEmitter(*instance);
        mEmitters.push_back(std::move(instance));
    }

    mElapsedTimer.restart();
    mLastFrameNs = -cFrameIntervalNs;
    stepFrame();
    mTimer.start();
}

void PreviewViewport::stepFrame() {
    if (mSystem == nullptr) {
        return;
    }
    const qint64 now = mElapsedTimer.nsecsElapsed();
    if (now - mLastFrameNs < cFrameIntervalNs) {
        return;
    }
    mLastFrameNs = now;
    mSystem->calc();
    update();
}

std::size_t PreviewViewport::lastFrameParticleCount() const {
    const Ptcl::Engine::EmitterDrawFrame& frame = mSystem->lastFrame();
    std::size_t count = 0;
    for (const auto& batch : frame.batches) {
        count += batch.particles.size();
    }
    return count;
}

// ========================================================================== //


void PreviewViewport::initializeGL() {
    initializeOpenGLFunctions();
    mIsEs = QOpenGLContext::currentContext()->isOpenGLES();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this,
        &PreviewViewport::cleanupGlResources);

    std::fprintf(stderr, "[viewport] GL_VERSION=%s GLSL=%s es=%d\n",
        reinterpret_cast<const char*>(glGetString(GL_VERSION)),
        reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)),
        mIsEs ? 1 : 0);

    ensureGlResources();
}

void PreviewViewport::cleanupGlResources() {
    if (!mGlReady) {
        return;
    }
    for (const auto& [texture, id] : mTextureCache) {
        (void)texture;
        if (id != 0) {
            glDeleteTextures(1, &id);
        }
    }
    mTextureCache.clear();
    if (mProgram) {
        mProgram->release();
        delete mProgram;
        mProgram = nullptr;
    }
    if (mScreenProgram) {
        mScreenProgram->release();
        delete mScreenProgram;
        mScreenProgram = nullptr;
    }
    if (mGridProgram) {
        mGridProgram->release();
        delete mGridProgram;
        mGridProgram = nullptr;
    }
    if (mStripeProgram) {
        mStripeProgram->release();
        delete mStripeProgram;
        mStripeProgram = nullptr;
    }
    mQuadVao.destroy();
    mQuadVbo.destroy();
    mStripeVao.destroy();
    mStripeVbo.destroy();
    mGridVao.destroy();
    mGridVbo.destroy();
    mAxisVao.destroy();
    mAxisVbo.destroy();
    mGridVertexCount = 0;
    mAxisVertexCount = 0;
    mGlReady = false;
}

void PreviewViewport::ensureGlResources() {
    if (mGlReady) {
        return;
    }

    mProgram = new QOpenGLShaderProgram(this);
    if (!mProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, mIsEs ? cVertShaderEs : cVertShader) ||
        !mProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, mIsEs ? cFragShaderEs : cFragShader) ||
        !mProgram->link()) {
        std::fprintf(stderr, "[viewport] shader compile/link failed: %s\n",
            mProgram->log().toUtf8().constData());
    }

    mScreenProgram = new QOpenGLShaderProgram(this);
    if (!mScreenProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
            mIsEs ? cScreenVertShaderEs : cScreenVertShader) ||
        !mScreenProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            mIsEs ? cScreenFragShaderEs : cScreenFragShader) ||
        !mScreenProgram->link()) {
        std::fprintf(stderr, "[viewport] screen shader compile/link failed: %s\n",
            mScreenProgram->log().toUtf8().constData());
    }

    static const std::array<GLfloat, 12> cQuad = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };

    mQuadVao.create();
    mQuadVbo.create();

    mQuadVao.bind();
    mQuadVbo.bind();
    mQuadVbo.allocate(cQuad.data(), static_cast<int>(sizeof(cQuad)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    mQuadVbo.release();
    mQuadVao.release();

    mStripeProgram = new QOpenGLShaderProgram(this);
    if (!mStripeProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
            mIsEs ? cStripeVertShaderEs : cStripeVertShader) ||
        !mStripeProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            mIsEs ? cFragShaderEs : cFragShader) ||
        !mStripeProgram->link()) {
        std::fprintf(stderr, "[viewport] stripe shader compile/link failed: %s\n",
            mStripeProgram->log().toUtf8().constData());
    }

    mStripeVao.create();
    mStripeVbo.create();
    mStripeVao.bind();
    mStripeVbo.bind();
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(StripeGLVertex),
        reinterpret_cast<const void*>(offsetof(StripeGLVertex, x)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(StripeGLVertex),
        reinterpret_cast<const void*>(offsetof(StripeGLVertex, u)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(StripeGLVertex),
        reinterpret_cast<const void*>(offsetof(StripeGLVertex, alpha)));
    mStripeVbo.release();
    mStripeVao.release();

    mGridProgram = new QOpenGLShaderProgram(this);
    if (!mGridProgram->addShaderFromSourceCode(QOpenGLShader::Vertex,
            mIsEs ? cGridVertShaderEs : cGridVertShader) ||
        !mGridProgram->addShaderFromSourceCode(QOpenGLShader::Fragment,
            mIsEs ? cGridFragShaderEs : cGridFragShader) ||
        !mGridProgram->link()) {
        std::fprintf(stderr, "[viewport] grid shader compile/link failed: %s\n",
            mGridProgram->log().toUtf8().constData());
    }

    const int extent = 256;
    std::vector<GLfloat> grid;
    grid.reserve((2 * extent + 1) * 2 * 2 * 3);
    for (int i = -extent; i <= extent; ++i) {
        if (i == 0) {
            continue;
        }
        const float x = static_cast<GLfloat>(i);
        grid.push_back(x);
        grid.push_back(0.0f);
        grid.push_back(static_cast<GLfloat>(-extent));
        grid.push_back(x);
        grid.push_back(0.0f);
        grid.push_back(static_cast<GLfloat>(extent));
        grid.push_back(static_cast<GLfloat>(-extent));
        grid.push_back(0.0f);
        grid.push_back(x);
        grid.push_back(static_cast<GLfloat>(extent));
        grid.push_back(0.0f);
        grid.push_back(x);
    }
    mGridVertexCount = static_cast<GLsizei>(grid.size() / 3);

    mGridVao.create();
    mGridVbo.create();
    mGridVao.bind();
    mGridVbo.bind();
    mGridVbo.allocate(grid.data(), static_cast<int>(grid.size() * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    mGridVbo.release();
    mGridVao.release();

    const std::array<GLfloat, 12> axes = {
        static_cast<GLfloat>(-extent), 0.0f, 0.0f,
        static_cast<GLfloat>(extent), 0.0f, 0.0f,
        0.0f, 0.0f, static_cast<GLfloat>(-extent),
        0.0f, 0.0f, static_cast<GLfloat>(extent),
    };
    mAxisVertexCount = 4;

    mAxisVao.create();
    mAxisVbo.create();
    mAxisVao.bind();
    mAxisVbo.bind();
    mAxisVbo.allocate(axes.data(), static_cast<int>(axes.size() * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    mAxisVbo.release();
    mAxisVao.release();

    mGlReady = true;
}

void PreviewViewport::paintGL() {
    glViewport(0, 0, width(), height());
    glClearColor(0.09f, 0.09f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!mGlReady || !mProgram) {
        return;
    }

    const Mat4 viewProj = buildViewProj(mYaw, mPitch, mTarget, mZoom, width(), height());
    const CameraBasis cameraBasis = makeCameraBasis(mYaw, mPitch, mTarget);

    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

    if (mGridProgram && mGridVertexCount > 0) {
        const int gridVp = mGridProgram->uniformLocation("uViewProj");
        const int gridCol = mGridProgram->uniformLocation("uColor");
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

        mGridProgram->bind();
        glUniformMatrix4fv(gridVp, 1, GL_FALSE, viewProj.data());

        mGridProgram->setUniformValue(gridCol, 0.25f, 0.29f, 0.33f, 1.0f);
        mGridVao.bind();
        glDrawArrays(GL_LINES, 0, mGridVertexCount);
        mGridVao.release();

        mGridProgram->setUniformValue(gridCol, 0.62f, 0.64f, 0.67f, 1.0f);
        mAxisVao.bind();
        glDrawArrays(GL_LINES, 0, mAxisVertexCount);
        mAxisVao.release();

        mGridProgram->release();
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_FALSE);

    mProgram->bind();

    const int uCenter = mProgram->uniformLocation("uCenter");
    const int uAxisX = mProgram->uniformLocation("uAxisX");
    const int uAxisY = mProgram->uniformLocation("uAxisY");
    const int uViewProj = mProgram->uniformLocation("uViewProj");
    const int uCfg = mProgram->uniformLocation("uCfg");
    const int uConstantColor = mProgram->uniformLocation("uConstantColor");
    const int uPrimaryColor = mProgram->uniformLocation("uPrimaryColor");
    const int uHasTex = mProgram->uniformLocation("uHasTex");
    const int uUvOffset = mProgram->uniformLocation("uUvOffset");
    const int uUvScale = mProgram->uniformLocation("uUvScale");
    const int uTexel = mProgram->uniformLocation("uTexel");

    glUniformMatrix4fv(uViewProj, 1, GL_FALSE, viewProj.data());

    glActiveTexture(GL_TEXTURE0);
    mQuadVao.bind();

    const auto& batches = mSystem->lastFrame().batches;
    std::size_t drawCount = 0;
    for (const auto& batch : batches) {
        const std::array<GLint, 32> cfgData = buildCombinerCfg(batch.state.combinerFunction);
        mProgram->setUniformValueArray(uCfg, cfgData.data(), static_cast<int>(cfgData.size()));

        setupRenderState(*this, batch.state.blendFunction, batch.state.depthFunction);

        const Ptcl::Texture* texture = nullptr;
        if (batch.state.textureHandle != nullptr && batch.state.textureHandle->isValid()) {
            texture = batch.state.textureHandle->get();
        }
        const bool hasTexture = (texture != nullptr && !texture->isPlaceholder());
        if (hasTexture) {
            const QImage source = texture->textureData();
            if (!source.isNull()) {
                mProgram->setUniformValue(uTexel,
                    1.0f / static_cast<GLfloat>(source.width()),
                    1.0f / static_cast<GLfloat>(source.height()));
            } else {
                mProgram->setUniformValue(uTexel, 0.0f, 0.0f);
            }
        } else {
            mProgram->setUniformValue(uTexel, 0.0f, 0.0f);
        }

        mProgram->setUniformValue(uHasTex, hasTexture ? 1 : 0);
        if (hasTexture) {
            GLuint texId = 0;
            const auto cacheIt = mTextureCache.find(texture);
            if (cacheIt != mTextureCache.end()) {
                texId = cacheIt->second;
            } else {
                texId = uploadTexture(*texture);
                if (texId != 0) {
                    mTextureCache.emplace(texture, texId);
                }
            }
            if (texId != 0) {
                glBindTexture(GL_TEXTURE_2D, texId);
                const GLint wrapS = (batch.state.textureWrapS == Ptcl::TextureWrap::Repeat) ? GL_REPEAT
                    : (batch.state.textureWrapS == Ptcl::TextureWrap::MirroredRepeat) ? GL_MIRRORED_REPEAT
                    : GL_CLAMP_TO_EDGE;
                const GLint wrapT = (batch.state.textureWrapT == Ptcl::TextureWrap::Repeat) ? GL_REPEAT
                    : (batch.state.textureWrapT == Ptcl::TextureWrap::MirroredRepeat) ? GL_MIRRORED_REPEAT
                    : GL_CLAMP_TO_EDGE;
                const GLint filter = (batch.state.textureFilter == Ptcl::TextureFilter::Nearest)
                    ? GL_NEAREST : GL_LINEAR;
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
            } else {
                mProgram->setUniformValue(uHasTex, 0);
            }
        }

        Gfx::Color constant = batch.state.color;
        if (!hasTexture && constant.r() <= 0.0f && constant.g() <= 0.0f && constant.b() <= 0.0f) {
            constant = Gfx::Color{0.9f, 0.9f, 0.9f, 1.0f};
        }
        mProgram->setUniformValue(uConstantColor,
            static_cast<GLfloat>(constant.r()),
            static_cast<GLfloat>(constant.g()),
            static_cast<GLfloat>(constant.b()),
            1.0f);

        for (const auto& particle : batch.particles) {
            const Math::Vector3f& pos = particle.positionDiff;

            Math::Vector3f axisX;
            Math::Vector3f axisY;
            computeParticleAxes(batch.state, particle, cameraBasis, axisX, axisY);

            const Math::Vector3f pivotOffset = Math::VectorCalc::add(
                Math::VectorCalc::multScalar(axisX, batch.state.rotationBasis.getX()),
                Math::VectorCalc::multScalar(axisY, batch.state.rotationBasis.getY()));
            const Math::Vector3f center = Math::VectorCalc::add(pos, pivotOffset);
            mProgram->setUniformValue(uCenter,
                static_cast<GLfloat>(center.getX()),
                static_cast<GLfloat>(center.getY()),
                static_cast<GLfloat>(center.getZ()));
            mProgram->setUniformValue(uAxisX,
                static_cast<GLfloat>(axisX.getX()),
                static_cast<GLfloat>(axisX.getY()),
                static_cast<GLfloat>(axisX.getZ()));
            mProgram->setUniformValue(uAxisY,
                static_cast<GLfloat>(axisY.getX()),
                static_cast<GLfloat>(axisY.getY()),
                static_cast<GLfloat>(axisY.getZ()));
            mProgram->setUniformValue(uPrimaryColor,
                static_cast<GLfloat>(particle.color.r()),
                static_cast<GLfloat>(particle.color.g()),
                static_cast<GLfloat>(particle.color.b()),
                static_cast<GLfloat>(particle.alpha));
            mProgram->setUniformValue(uUvOffset,
                static_cast<GLfloat>(particle.uvOffset.getX()),
                static_cast<GLfloat>(particle.uvOffset.getY()));
            mProgram->setUniformValue(uUvScale,
                static_cast<GLfloat>(particle.uvSubOffset.getX()),
                static_cast<GLfloat>(particle.uvSubOffset.getY()));

            glDrawArrays(GL_TRIANGLES, 0, 6);
            ++drawCount;
        }
    }
    mLastDrawCount = drawCount;

    mQuadVao.release();
    mProgram->release();

    if (mStripeProgram) {
        mStripeProgram->bind();

        const int suViewProj = mStripeProgram->uniformLocation("uViewProj");
        const int suCfg = mStripeProgram->uniformLocation("uCfg");
        const int suConstantColor = mStripeProgram->uniformLocation("uConstantColor");
        const int suPrimaryColor = mStripeProgram->uniformLocation("uPrimaryColor");
        const int suHasTex = mStripeProgram->uniformLocation("uHasTex");
        const int suTex = mStripeProgram->uniformLocation("uTex");
        glUniformMatrix4fv(suViewProj, 1, GL_FALSE, viewProj.data());
        glActiveTexture(GL_TEXTURE0);
        mStripeProgram->setUniformValue(suTex, 0);

        for (const auto& stripeEntry : mSystem->lastFrame().stripes) {
            const auto& state = stripeEntry.state;
            const auto& points = stripeEntry.stripe.points;
            if (points.size() < 2) {
                continue;
            }

            const auto cfgData = buildCombinerCfg(state.combinerFunction);
            mStripeProgram->setUniformValueArray(suCfg, cfgData.data(), static_cast<int>(cfgData.size()));
            setupRenderState(*this, state.blendFunction, state.depthFunction);

            const Ptcl::Texture* texture = nullptr;
            if (state.textureHandle != nullptr && state.textureHandle->isValid()) {
                texture = state.textureHandle->get();
            }
            const bool hasTexture = (texture != nullptr && !texture->isPlaceholder());
            mStripeProgram->setUniformValue(suHasTex, hasTexture ? 1 : 0);
            if (hasTexture) {
                GLuint texId = 0;
                const auto cacheIt = mTextureCache.find(texture);
                if (cacheIt != mTextureCache.end()) {
                    texId = cacheIt->second;
                } else {
                    texId = uploadTexture(*texture);
                    if (texId != 0) {
                        mTextureCache.emplace(texture, texId);
                    }
                }
                if (texId != 0) {
                    glBindTexture(GL_TEXTURE_2D, texId);
                    const GLint wrapS = (state.textureWrapS == Ptcl::TextureWrap::Repeat) ? GL_REPEAT
                        : (state.textureWrapS == Ptcl::TextureWrap::MirroredRepeat) ? GL_MIRRORED_REPEAT
                        : GL_CLAMP_TO_EDGE;
                    const GLint wrapT = (state.textureWrapT == Ptcl::TextureWrap::Repeat) ? GL_REPEAT
                        : (state.textureWrapT == Ptcl::TextureWrap::MirroredRepeat) ? GL_MIRRORED_REPEAT
                        : GL_CLAMP_TO_EDGE;
                    const GLint filter = (state.textureFilter == Ptcl::TextureFilter::Nearest)
                        ? GL_NEAREST : GL_LINEAR;
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
                } else {
                    mStripeProgram->setUniformValue(suHasTex, 0);
                }
            }

            Gfx::Color constant = state.secondaryColor;
            if (!hasTexture && constant.r() <= 0.0f && constant.g() <= 0.0f && constant.b() <= 0.0f) {
                constant = Gfx::Color{0.9f, 0.9f, 0.9f, 1.0f};
            }
            mStripeProgram->setUniformValue(suConstantColor,
                static_cast<GLfloat>(constant.r()),
                static_cast<GLfloat>(constant.g()),
                static_cast<GLfloat>(constant.b()),
                1.0f);
            mStripeProgram->setUniformValue(suPrimaryColor,
                static_cast<GLfloat>(state.color.r()),
                static_cast<GLfloat>(state.color.g()),
                static_cast<GLfloat>(state.color.b()),
                1.0f);

            std::vector<StripeGLVertex> vertices;
            vertices.reserve(points.size() * 2);
            for (const auto& point : points) {
                Math::Vector3f edge0;
                Math::Vector3f edge1;
                if (state.billboardType == Ptcl::BillboardType::Stripe) {
                    Math::Vector3f direction = point.outer;
                    if (direction.length() <= 0.0001f) {
                        direction = cameraBasis.up;
                    }
                    Math::VectorCalc::normalize(direction);
                    edge0 = Math::VectorCalc::cross(direction, cameraBasis.front);
                    if (edge0.length() <= 0.0001f) {
                        edge0 = cameraBasis.right;
                    }
                    Math::VectorCalc::normalize(edge0);
                    edge0 = Math::VectorCalc::multScalar(edge0, point.scale * 0.5f);
                    edge1 = Math::VectorCalc::multScalar(edge0, -1.0f);
                } else {
                    edge0 = Math::VectorCalc::multScalar(point.outer, 0.5f);
                    edge1 = Math::VectorCalc::multScalar(edge0, -1.0f);
                }

                const Math::Vector3f p0 = Math::VectorCalc::add(point.position, edge0);
                const Math::Vector3f p1 = Math::VectorCalc::add(point.position, edge1);
                const float u0 = point.uv.getX() + point.uvScale.getX();
                const float u1 = point.uv.getX();
                const float v = point.uv.getY() + point.t * point.uvScale.getY();
                vertices.push_back({p0.getX(), p0.getY(), p0.getZ(), u0, v, point.alpha});
                vertices.push_back({p1.getX(), p1.getY(), p1.getZ(), u1, v, point.alpha});
            }

            mStripeVbo.bind();
            mStripeVbo.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(StripeGLVertex)));
            mStripeVbo.release();
            mStripeVao.bind();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(vertices.size()));
            mStripeVao.release();
        }

        mStripeProgram->release();
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    if (mScreenProgram) {
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
        mScreenProgram->bind();
        mQuadVao.bind();
        glDrawArrays(GL_TRIANGLES, 0, 6);
        mQuadVao.release();
        mScreenProgram->release();
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
}

void PreviewViewport::resizeGL(int, int) {
    update();
}

void PreviewViewport::wheelEvent(QWheelEvent* event) {
    const bool zoomIn = event->angleDelta().y() > 0;
    const float zoomChange = zoomIn ? cZoomStep : -cZoomStep;
    mZoom += zoomChange * std::pow(mZoom, cZoomExponent);
    mZoom = qBound(cZoomMin, mZoom, cZoomMax);
    update();
    event->accept();
}

void PreviewViewport::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        mOrbitDragging = true;
        mLastMouse = event->pos();
        event->accept();
        return;
    }
    if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        mPanDragging = true;
        mLastMouse = event->pos();
        event->accept();
        return;
    }
    QOpenGLWidget::mousePressEvent(event);
}

void PreviewViewport::mouseMoveEvent(QMouseEvent* event) {
    if (mOrbitDragging) {
        const QPoint delta = event->pos() - mLastMouse;
        mYaw -= delta.x() * 0.01f;
        mPitch = qBound(-1.55f, mPitch + delta.y() * 0.01f, 1.55f);
        mLastMouse = event->pos();
        update();
        event->accept();
        return;
    }
    if (mPanDragging) {
        const QPoint delta = event->pos() - mLastMouse;
        const float visibleHalfHeight = std::tan(cVerticalFov * 0.5f) * cCameraDistance / mZoom;
        const float worldPerPixel = (2.0f * visibleHalfHeight) / static_cast<float>(height());

        const float cyaw = std::cos(mYaw);
        const float syaw = std::sin(mYaw);
        const float cpitch = std::cos(mPitch);
        const float spitch = std::sin(mPitch);

        const float fwdX = -cpitch * syaw;
        const float fwdY = -spitch;
        const float fwdZ = -cpitch * cyaw;

        float rx = fwdZ;
        float ry = 0.0f;
        float rz = -fwdX;
        const float rl = std::sqrt(rx * rx + rz * rz);
        if (rl > 0.0f) {
            rx /= rl;
            rz /= rl;
        }

        const float ux = ry * fwdZ - rz * fwdY;
        const float uy = rz * fwdX - rx * fwdZ;
        const float uz = rx * fwdY - ry * fwdX;

        const float dx = static_cast<float>(-delta.x()) * worldPerPixel;
        const float dy = static_cast<float>(delta.y()) * worldPerPixel;

        mTarget.setX(mTarget.getX() + rx * dx + ux * dy);
        mTarget.setY(mTarget.getY() + ry * dx + uy * dy);
        mTarget.setZ(mTarget.getZ() + rz * dx + uz * dy);

        mLastMouse = event->pos();
        update();
        event->accept();
        return;
    }
    QOpenGLWidget::mouseMoveEvent(event);
}

void PreviewViewport::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        mOrbitDragging = false;
        event->accept();
        return;
    }
    if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
        mPanDragging = false;
        event->accept();
        return;
    }
    QOpenGLWidget::mouseReleaseEvent(event);
}


// ========================================================================== //


} // namespace PtclTool
