#include "editor/viewport/viewportGrid.h"

#include "editor/viewport/viewportCamera.h"
#include "gfx/glGeometry.h"

#include <QDebug>

#include <algorithm>
#include <array>
#include <cmath>


namespace PtclTool {


// ========================================================================== //


static constexpr const char* cGridVertShader = R"(#version 150 core
in vec2 aPos;
out vec2 vNdc;
void main() {
    vNdc = aPos * 2.0;
    gl_Position = vec4(vNdc, 0.0, 1.0);
}
)";

static constexpr const char* cGridFragShader = R"(#version 150 core
#extension GL_OES_standard_derivatives : enable
in vec2 vNdc;
out vec4 fragColor;

uniform mat4 uInverseViewProjection;
uniform vec3 uCameraPosition;
uniform vec3 uViewForward;
uniform vec3 uPlaneNormal;
uniform vec3 uPlanePoint;
uniform vec3 uPlaneU;
uniform vec3 uPlaneV;
uniform vec3 uAxisColorU;
uniform vec3 uAxisColorV;
uniform int uPerspective;
uniform float uFadeStart;
uniform float uFadeEnd;
uniform float uPixelRatio;
uniform float uGridCellSize;

const vec3 cMinorColor = vec3(0.42, 0.47, 0.55);
const vec3 cMajorColor = vec3(0.62, 0.67, 0.76);
const float cGridMinPixels = 32.0;
const float cGridStrength = 0.85;
const float cGridFineAlpha = 0.35;
const float cGridMiddleAlpha = 0.75;
const float cGridEpsilon = 1.0e-7;

float lineMask(float coordinate, float spacing, float worldPerPixel, float widthPixels) {
    float cells = coordinate / spacing;
    float distanceToLine = abs(fract(cells - 0.5) - 0.5) * spacing;
    return 1.0 - smoothstep(0.0, max(worldPerPixel, cGridEpsilon) * widthPixels, distanceToLine);
}

float axisMask(float coordinate, float worldPerPixel, float widthPixels) {
    float distancePixels = abs(coordinate) / max(worldPerPixel, cGridEpsilon);
    return 1.0 - smoothstep(widthPixels, widthPixels + uPixelRatio, distancePixels);
}

void main() {
    vec4 nearHomogeneous = uInverseViewProjection * vec4(vNdc, -1.0, 1.0);
    vec4 farHomogeneous = uInverseViewProjection * vec4(vNdc, 1.0, 1.0);
    if (abs(nearHomogeneous.w) < cGridEpsilon || abs(farHomogeneous.w) < cGridEpsilon) {
        discard;
    }

    vec3 nearPoint = nearHomogeneous.xyz / nearHomogeneous.w;
    vec3 farPoint = farHomogeneous.xyz / farHomogeneous.w;
    vec3 ray = farPoint - nearPoint;

    float denominator = dot(uPlaneNormal, ray);
    if (abs(denominator) < cGridEpsilon) {
        discard;
    }

    float t = dot(uPlaneNormal, uPlanePoint - nearPoint) / denominator;
    vec3 worldPosition = nearPoint + ray * t;

    vec3 cameraDelta = worldPosition - uCameraPosition;
    float cameraDistanceSq = dot(cameraDelta, cameraDelta);
    float maxDistanceSq = uFadeEnd * uFadeEnd;

    if (uPerspective == 1) {
        if (dot(uViewForward, cameraDelta) <= 0.0
            || !(cameraDistanceSq >= 1.0)
            || !(cameraDistanceSq <= maxDistanceSq)) {
            discard;
        }
    }
    vec2 grid = vec2(
        dot(worldPosition - uPlanePoint, uPlaneU),
        dot(worldPosition - uPlanePoint, uPlaneV)
    );
    vec2 worldPerPixel = fwidth(grid);

    float cellSize = max(uGridCellSize, cGridEpsilon);
    float lod = log2(max(max(worldPerPixel.x, worldPerPixel.y), cGridEpsilon)
        * cGridMinPixels * uPixelRatio / cellSize) / log2(10.0);

    float level = max(0.0, floor(lod));
    float fraction = clamp(lod - level, 0.0, 1.0);

    float spacingFine = cellSize * pow(10.0, level);
    float spacingMiddle = spacingFine * 10.0;
    float spacingCoarse = spacingMiddle * 10.0;

    float fineLine = max(
        lineMask(grid.x, spacingFine, worldPerPixel.x, 0.8 * uPixelRatio),
        lineMask(grid.y, spacingFine, worldPerPixel.y, 0.8 * uPixelRatio)
    );
    float middleLine = max(
        lineMask(grid.x, spacingMiddle, worldPerPixel.x, 0.9 * uPixelRatio),
        lineMask(grid.y, spacingMiddle, worldPerPixel.y, 0.9 * uPixelRatio)
    );
    float coarseLine = max(
        lineMask(grid.x, spacingCoarse, worldPerPixel.x, 1.1 * uPixelRatio),
        lineMask(grid.y, spacingCoarse, worldPerPixel.y, 1.1 * uPixelRatio)
    );

    float fineAlpha = fineLine * cGridFineAlpha * (1.0 - fraction);
    float middleAlpha = middleLine * mix(cGridMiddleAlpha, cGridFineAlpha, fraction);
    float coarseAlpha = coarseLine * cGridMiddleAlpha * fraction;
    float gridAlpha = max(max(fineAlpha, middleAlpha), coarseAlpha);
    vec3 gridColor = mix(cMinorColor, cMajorColor,
        smoothstep(cGridFineAlpha, cGridMiddleAlpha, gridAlpha));

    float fade = 1.0;
    if (uPerspective != 0) {
        fade = 1.0 - smoothstep(uFadeStart, uFadeEnd, distance(worldPosition, uCameraPosition));
    }

    float alpha = gridAlpha * cGridStrength * fade;
    vec3 color = gridColor;

    float axisU = axisMask(grid.x, worldPerPixel.x, uPixelRatio);
    float axisV = axisMask(grid.y, worldPerPixel.y, uPixelRatio);

    if (axisU > 0.0) {
        color = mix(color, uAxisColorU, axisU);
        alpha = max(alpha, axisU * fade);
    }

    if (axisV > 0.0) {
        color = mix(color, uAxisColorV, axisV);
        alpha = max(alpha, axisV * fade);
    }

    if (!(alpha > 0.001)) {
        discard;
    }

    fragColor = vec4(color, alpha);
}
)";

static constexpr f32 cGridFadeStart = 1000.0f;
static constexpr f32 cGridFadeEnd = 1900.0f;

static constexpr std::array<GLfloat, 3> cAxisXColor = {0.95f, 0.24f, 0.28f};
static constexpr std::array<GLfloat, 3> cAxisYColor = {0.42f, 0.82f, 0.30f};
static constexpr std::array<GLfloat, 3> cAxisZColor = {0.25f, 0.50f, 0.98f};

static const Math::Vector3f cWorldUp{0.0f, 1.0f, 0.0f};


void ViewportGrid::initialize(QOpenGLFunctions_2_0& gl) {
    if (mInitialized) {
        return;
    }

    auto program = std::make_unique<QOpenGLShaderProgram>();
    program->bindAttributeLocation("aPos", 0);
    if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex, cGridVertShader)) {
        qWarning() << "[viewport] Failed to compile grid vertex shader:" << program->log();
        return;
    }
    if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment, cGridFragShader)) {
        qWarning() << "[viewport] Failed to compile grid fragment shader:" << program->log();
        return;
    }
    if (!program->link()) {
        qWarning() << "[viewport] Failed to link grid shader:" << program->log();
        return;
    }

    const auto inverseViewProjectionLocation = program->uniformLocation("uInverseViewProjection");
    const auto cameraPositionLocation        = program->uniformLocation("uCameraPosition");
    const auto viewForwardLocation           = program->uniformLocation("uViewForward");
    const auto planeNormalLocation           = program->uniformLocation("uPlaneNormal");
    const auto planePointLocation            = program->uniformLocation("uPlanePoint");
    const auto planeULocation                = program->uniformLocation("uPlaneU");
    const auto planeVLocation                = program->uniformLocation("uPlaneV");
    const auto axisColorULocation            = program->uniformLocation("uAxisColorU");
    const auto axisColorVLocation            = program->uniformLocation("uAxisColorV");
    const auto perspectiveLocation           = program->uniformLocation("uPerspective");
    const auto fadeStartLocation             = program->uniformLocation("uFadeStart");
    const auto fadeEndLocation               = program->uniformLocation("uFadeEnd");
    const auto pixelRatioLocation            = program->uniformLocation("uPixelRatio");
    const auto gridCellSizeLocation          = program->uniformLocation("uGridCellSize");

    if (inverseViewProjectionLocation < 0 ||
        cameraPositionLocation < 0 ||
        viewForwardLocation < 0 ||
        planeNormalLocation < 0 ||
        planePointLocation < 0 ||
        planeULocation < 0 ||
        planeVLocation < 0 ||
        axisColorULocation < 0 ||
        axisColorVLocation < 0 ||
        perspectiveLocation < 0 ||
        fadeStartLocation < 0 ||
        fadeEndLocation < 0 ||
        pixelRatioLocation < 0 ||
        gridCellSizeLocation < 0) {
        qWarning() << "[viewport] Grid shader uniforms were not found.";
        return;
    }

    Gfx::createUnitQuad(gl, mQuadVbo, mQuadVao);

    mInverseViewProjectionLocation = inverseViewProjectionLocation;
    mCameraPositionLocation = cameraPositionLocation;
    mViewForwardLocation = viewForwardLocation;
    mPlaneNormalLocation = planeNormalLocation;
    mPlanePointLocation = planePointLocation;
    mPlaneULocation = planeULocation;
    mPlaneVLocation = planeVLocation;
    mAxisColorULocation = axisColorULocation;
    mAxisColorVLocation = axisColorVLocation;
    mPerspectiveLocation = perspectiveLocation;
    mFadeStartLocation = fadeStartLocation;
    mFadeEndLocation = fadeEndLocation;
    mPixelRatioLocation = pixelRatioLocation;
    mGridCellSizeLocation = gridCellSizeLocation;
    mProgram = std::move(program);
    mInitialized = true;
}

void ViewportGrid::draw(QOpenGLFunctions_2_0& gl, const ViewportCamera& camera, float pixelRatio, float gridCellSize) {
    if (!mInitialized || mProgram == nullptr) {
        return;
    }

    const Gfx::ViewBasis& basis = camera.viewBasis();

    const bool verticalViewPlane =
        (camera.projectionType() == ViewportCamera::ProjectionType::Orthographic) &&
        (std::abs(basis.forward.y) < 1.0e-4f);

    Math::Vector3f planeNormal;
    Math::Vector3f planeU;
    Math::Vector3f planeV;
    const GLfloat* axisColorU = nullptr;
    const GLfloat* axisColorV = nullptr;
    std::array<GLfloat, 3> blendedAxisColor{};

    if (verticalViewPlane) {
        planeNormal = {basis.forward.x, 0.0f, basis.forward.z};
        Math::VectorCalc::normalize(planeNormal);
        planeU = Math::VectorCalc::cross(cWorldUp, planeNormal);
        Math::VectorCalc::normalize(planeU);
        planeV = cWorldUp;

        axisColorU = cAxisYColor.data();

        const f32 axisXWeight = std::abs(planeU.x);
        const f32 axisZWeight = std::abs(planeU.z);
        const f32 axisWeightSum = axisXWeight + axisZWeight;
        for (std::size_t i = 0; i < blendedAxisColor.size(); ++i) {
            blendedAxisColor[i] =
                (cAxisXColor[i] * axisXWeight + cAxisZColor[i] * axisZWeight) / axisWeightSum;
        }
        axisColorV = blendedAxisColor.data();
    } else {
        planeNormal = cWorldUp;
        planeU = {1.0f, 0.0f, 0.0f};
        planeV = {0.0f, 0.0f, 1.0f};

        axisColorU = cAxisZColor.data();
        axisColorV = cAxisXColor.data();
    }

    GLint previousProgram = 0;
    GLint previousDepthFunction = GL_LESS;
    const GLboolean depthTestEnabled = gl.glIsEnabled(GL_DEPTH_TEST);
    const GLboolean cullFaceEnabled = gl.glIsEnabled(GL_CULL_FACE);
    GLboolean depthWriteEnabled = GL_TRUE;
    gl.glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
    if (depthTestEnabled) {
        gl.glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunction);
    }
    gl.glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWriteEnabled);

    mProgram->bind();
    gl.glUniformMatrix4fv(mInverseViewProjectionLocation, 1, GL_TRUE, camera.inverseViewProjection().data());


    const Math::Vector3f cameraPosition = camera.position();
    const f32 viewDistance = camera.viewDistance();
    const f32 fadeStart = std::max(cGridFadeStart, viewDistance * 2.5f);
    const f32 fadeEnd = std::max(cGridFadeEnd, viewDistance * 4.75f);

    gl.glUniform3f(mCameraPositionLocation, cameraPosition.x, cameraPosition.y, cameraPosition.z);
    gl.glUniform3f(mViewForwardLocation, basis.forward.x, basis.forward.y, basis.forward.z);
    gl.glUniform3f(mPlaneNormalLocation, planeNormal.x, planeNormal.y, planeNormal.z);
    gl.glUniform3f(mPlanePointLocation, 0.0f, 0.0f, 0.0f);
    gl.glUniform3f(mPlaneULocation, planeU.x, planeU.y, planeU.z);
    gl.glUniform3f(mPlaneVLocation, planeV.x, planeV.y, planeV.z);
    gl.glUniform3fv(mAxisColorULocation, 1, axisColorU);
    gl.glUniform3fv(mAxisColorVLocation, 1, axisColorV);
    gl.glUniform1i(mPerspectiveLocation, camera.projectionType() == ViewportCamera::ProjectionType::Perspective ? 1 : 0);
    gl.glUniform1f(mFadeStartLocation, fadeStart);
    gl.glUniform1f(mFadeEndLocation, fadeEnd);
    gl.glUniform1f(mPixelRatioLocation, std::max(pixelRatio, 1.0f));
    gl.glUniform1f(mGridCellSizeLocation, std::max(gridCellSize, 0.0001f));

    gl.glDisable(GL_DEPTH_TEST);
    gl.glDepthMask(GL_FALSE);
    gl.glDisable(GL_CULL_FACE);

    mQuadVao.bind();
    gl.glDrawArrays(GL_TRIANGLES, 0, Gfx::unitQuadVertexCount);
    mQuadVao.release();
    mProgram->release();

    gl.glUseProgram(static_cast<GLuint>(previousProgram));
    gl.glDepthMask(depthWriteEnabled);
    if (cullFaceEnabled) {
        gl.glEnable(GL_CULL_FACE);
    } else {
        gl.glDisable(GL_CULL_FACE);
    }
    if (depthTestEnabled) {
        gl.glEnable(GL_DEPTH_TEST);
        gl.glDepthFunc(static_cast<GLenum>(previousDepthFunction));
    } else {
        gl.glDisable(GL_DEPTH_TEST);
    }
}

void ViewportGrid::cleanup() {
    mProgram.reset();

    mQuadVao.destroy();
    mQuadVbo.destroy();

    mInverseViewProjectionLocation = -1;
    mCameraPositionLocation = -1;
    mViewForwardLocation = -1;
    mPlaneNormalLocation = -1;
    mPlanePointLocation = -1;
    mPlaneULocation = -1;
    mPlaneVLocation = -1;
    mAxisColorULocation = -1;
    mAxisColorVLocation = -1;
    mPerspectiveLocation = -1;
    mFadeStartLocation = -1;
    mFadeEndLocation = -1;
    mPixelRatioLocation = -1;
    mGridCellSizeLocation = -1;
    mInitialized = false;
}


// ========================================================================== //


} // namespace PtclTool
