#include "editor/viewport/previewViewport.h"

#include "editor/components/combinerPreviewWidget.h"
#include "ptcl/ptclEmitterSet.h"
#include "ptcl/ptclTexture.h"

#include <QSurfaceFormat>
#include <QWheelEvent>

#include <array>


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
out vec2 vUv;
void main() {
    vUv = uUvOffset + (aPos + 0.5) * uUvScale;
    vec3 world = uCenter + aPos.x * uAxisX + aPos.y * uAxisY;
    gl_Position = uViewProj * vec4(world, 1.0);
}
)";

constexpr const char* cFragShader = R"(#version 330 core
in vec2 vUv;
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
        vec2 c = vUv - vec2(0.5);
        float m = step(length(c), 0.5);
        tex = vec4(vec3(1.0), m);
    }
    vec4 prev = vec4(0.0);
    vec3 cOut = evalColorStage(0, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(cOut, 0.0);
    cOut = evalColorStage(8, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, 0.0);
    float aOut = evalAlphaStage(16, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, aOut);
    aOut = evalAlphaStage(24, uConstantColor, uPrimaryColor, tex, prev);
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
out vec2 vUv;
void main() {
    vUv = uUvOffset + (aPos + 0.5) * uUvScale;
    vec3 world = uCenter + aPos.x * uAxisX + aPos.y * uAxisY;
    gl_Position = uViewProj * vec4(world, 1.0);
}
)";

constexpr const char* cFragShaderEs = R"(#version 300 es
precision highp float;
in vec2 vUv;
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
        vec2 c = vUv - vec2(0.5);
        float m = step(length(c), 0.5);
        tex = vec4(vec3(1.0), m);
    }
    vec4 prev = vec4(0.0);
    vec3 cOut = evalColorStage(0, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(cOut, 0.0);
    cOut = evalColorStage(8, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, 0.0);
    float aOut = evalAlphaStage(16, uConstantColor, uPrimaryColor, tex, prev);
    prev = vec4(0.0, 0.0, 0.0, aOut);
    aOut = evalAlphaStage(24, uConstantColor, uPrimaryColor, tex, prev);
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

Mat4 makeOrtho(float left, float right, float bottom, float top, float nearZ, float farZ) {
    Mat4 m{};
    m[0] = 2.0f / (right - left);
    m[5] = 2.0f / (top - bottom);
    m[10] = -2.0f / (farZ - nearZ);
    m[12] = -(right + left) / (right - left);
    m[13] = -(top + bottom) / (top - bottom);
    m[14] = -(farZ + nearZ) / (farZ - nearZ);
    m[15] = 1.0f;
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

constexpr float cCameraDistance = 10000.0f;
constexpr float cNear = 10.0f;
constexpr float cFar = 20000.0f;
constexpr float cHalfWidth = 200.0f;
constexpr float cHalfHeight = 120.0f;
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
    const float hx = particle.scale.getX() * 0.5f;
    const float hy = particle.scale.getY() * 0.5f;
    const auto& m = particle.matrix;

    switch (state.billboardType) {
    case Ptcl::BillboardType::BillboardY:
        axisX = Math::VectorCalc::multScalar(cam.right, hx);
        axisY = {0.0f, hy, 0.0f};
        break;

    case Ptcl::BillboardType::PolygonXY:
        axisX = {m(0, 3) * hx, m(1, 3) * hx, m(2, 3) * hx};
        axisY = {m(0, 2) * hy, m(1, 2) * hy, m(2, 2) * hy};
        break;

    case Ptcl::BillboardType::PolygonXZ:
        axisX = {m(0, 3) * hx, m(1, 3) * hx, m(2, 3) * hx};
        axisY = {m(0, 1) * hy, m(1, 1) * hy, m(2, 1) * hy};
        break;

    case Ptcl::BillboardType::VelLook:
    case Ptcl::BillboardType::VelLookPolygon: {
        Math::Vector3f dir = particle.followDirection;
        if (dir.length() <= 0.0001f) {
            dir = particle.positionDiff;
        }
        if (dir.length() <= 0.0001f) {
            dir = cam.up;
        }
        Math::VectorCalc::normalize(dir);

        axisY = Math::VectorCalc::multScalar(dir, hy);
        Math::Vector3f right = Math::VectorCalc::cross(cam.front, dir);
        if (right.length() <= 0.0001f) {
            right = cam.right;
        }
        Math::VectorCalc::normalize(right);
        axisX = Math::VectorCalc::multScalar(right, hx);
        break;
    }

    case Ptcl::BillboardType::Stripe:
    case Ptcl::BillboardType::ComplexStripe:
    case Ptcl::BillboardType::Primitive:
    default:
        axisX = {m(0, 3) * hx, m(1, 3) * hx, m(2, 3) * hx};
        axisY = {m(0, 2) * hy, m(1, 2) * hy, m(2, 2) * hy};
        break;

    case Ptcl::BillboardType::Billboard:
        axisX = Math::VectorCalc::multScalar(cam.right, hx);
        axisY = Math::VectorCalc::multScalar(cam.up, hy);
        break;
    }
}

Mat4 buildViewProj(float yaw, float pitch, const Math::Vector3f& target, float zoomFactor) {
    const float cyaw = std::cos(yaw);
    const float syaw = std::sin(yaw);
    const float cpitch = std::cos(pitch);
    const float spitch = std::sin(pitch);

    const float ex = target.getX() + cCameraDistance * cpitch * syaw;
    const float ey = target.getY() + cCameraDistance * spitch;
    const float ez = target.getZ() + cCameraDistance * cpitch * cyaw;

    const Mat4 view = makeLookAt(ex, ey, ez, target.getX(), target.getY(), target.getZ(), 0.0f, 1.0f, 0.0f);

    const float halfWidth = cHalfWidth / zoomFactor;
    const float halfHeight = cHalfHeight / zoomFactor;
    const Mat4 proj = makeOrtho(-halfWidth, halfWidth, -halfHeight, halfHeight, cNear, cFar);

    return multiplyMat4(proj, view);
}


} // namespace


// ========================================================================== //


PreviewViewport::PreviewViewport(QWidget* parent) :
    QOpenGLWidget{parent} {
    QSurfaceFormat fmt = format();
    fmt.setDepthBufferSize(24);
    setFormat(fmt);

    setMinimumSize(320, 240);
    mTimer.setTimerType(Qt::PreciseTimer);
    mTimer.setInterval(1);
    connect(&mTimer, &QTimer::timeout, this, &PreviewViewport::stepFrame);
}

PreviewViewport::~PreviewViewport() {
    mTimer.stop();
    disconnect(context(), nullptr, this, nullptr);
}

void PreviewViewport::setEmitterSet(const Ptcl::EmitterSet* emitterSet) {
    mEmitterSet = emitterSet;
    if (emitterSet == nullptr) {
        clearPreview();
        return;
    }
    rebuild(*emitterSet);
    mTimer.start();
    update();
}

void PreviewViewport::clearPreview() {
    mTimer.stop();
    mEmitterSet = nullptr;
    mEmitters.clear();
    mSetInstance = {};
    mSystem.reset();
    mFrame.batches.clear();
    mLastDrawCount = 0;
    update();
}

void PreviewViewport::rebuild(const Ptcl::EmitterSet& emitterSet) {
    mTimer.stop();
    mEmitters.clear();
    mSetInstance = {};
    mSystem = std::make_unique<Ptcl::Engine::PtclSystem>(8);
    mFrame.batches.clear();

    u32 seed = 0x13579BDFu;
    for (const auto& res : emitterSet.emitters()) {
        std::unique_ptr<Ptcl::Engine::EmitterInstance> instance =
            std::make_unique<Ptcl::Engine::EmitterInstance>(*res, mSetInstance, seed, 3, 21, res->emitRate());
        seed = seed * 1664525u + 1013904223u;

        mSetInstance.addEmitter(*instance);
        mSystem->addEmitter(*instance);
        mEmitters.push_back(std::move(instance));
    }

    mElapsedTimer.restart();
    mLastFrameNs = -cFrameIntervalNs;
    stepFrame();
    mTimer.start();
}

void PreviewViewport::beginCalc() {
    mFrame.batches.clear();
}

void PreviewViewport::setEmitterState(const Ptcl::Engine::EmitterState& state) {
    mFrame.batches.push_back({state, {}});
}

void PreviewViewport::emitParticle(const Ptcl::Engine::ParticleState& particle) {
    if (mFrame.batches.empty()) {
        return;
    }
    mFrame.batches.back().particles.push_back(particle);
}

void PreviewViewport::endCalc() {
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
    mSetInstance.setMtx(mSetInstance.SRT);
    mSystem->calc(*this);
    update();
}

std::size_t PreviewViewport::lastFrameParticleCount() const {
    std::size_t count = 0;
    for (const auto& batch : mFrame.batches) {
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
    mQuadVao.destroy();
    mQuadVbo.destroy();
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

    const Mat4 viewProj = buildViewProj(mYaw, mPitch, mTarget, mZoom);
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

    glUniformMatrix4fv(uViewProj, 1, GL_FALSE, viewProj.data());

    glActiveTexture(GL_TEXTURE0);
    mQuadVao.bind();

    const auto& batches = mFrame.batches;
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

            mProgram->setUniformValue(uCenter,
                static_cast<GLfloat>(pos.getX()),
                static_cast<GLfloat>(pos.getY()),
                static_cast<GLfloat>(pos.getZ()));
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
        const float worldPerPixel = (2.0f * (cHalfHeight / mZoom)) / static_cast<float>(height());

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
