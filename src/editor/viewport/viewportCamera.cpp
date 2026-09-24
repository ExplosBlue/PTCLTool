#include "editor/viewport/viewportCamera.h"

#include <cmath>


namespace PtclTool {


// ========================================================================== //


static constexpr f32 cCameraDistance = 400.0f;
static constexpr f32 cVerticalFov = std::numbers::pi_v<f32> / 4.0f;
static constexpr f32 cNear = 1.0f;
static constexpr f32 cFar = 2000.0f;
static constexpr f32 cZoomMin = 0.1f;
static constexpr f32 cZoomMax = 10.0f;
static constexpr f32 cZoomExponent = 1.1f;

static Math::Matrix44f makePerspective(f32 fovYRadians, f32 aspect, f32 nearZ, f32 farZ) {
    const f32 focal = 1.0f / std::tan(fovYRadians * 0.5f);
    const f32 depthScale = (farZ + nearZ) / (nearZ - farZ);
    const f32 depthOffset = (2.0f * farZ * nearZ) / (nearZ - farZ);
    return {{
        {focal / aspect, 0.0f,  0.0f,        0.0f},
        {0.0f,           focal, 0.0f,        0.0f},
        {0.0f,           0.0f,  depthScale, -1.0f},
        {0.0f,           0.0f,  depthOffset, 0.0f},
    }};
}


// ========================================================================== //


ViewportCamera::ViewportCamera() = default;

Math::Vector3f ViewportCamera::eye() const {
    return eye(basis());
}

Math::Vector3f ViewportCamera::eye(const Basis& camera) const {
    return Math::VectorCalc::add(mTarget, Math::VectorCalc::multScalar(camera.forward, -cCameraDistance));
}

Math::Matrix44f ViewportCamera::makeView() const {
    const auto camera = basis();
    const auto cameraEye = eye(camera);

    Math::Matrix44f view{};
    view(0, 0) = camera.right.getX();
    view(0, 1) = camera.right.getY();
    view(0, 2) = camera.right.getZ();
    view(0, 3) = -Math::VectorCalc::dot(camera.right, cameraEye);

    view(1, 0) = camera.up.getX();
    view(1, 1) = camera.up.getY();
    view(1, 2) = camera.up.getZ();
    view(1, 3) = -Math::VectorCalc::dot(camera.up, cameraEye);

    view(2, 0) = -camera.forward.getX();
    view(2, 1) = -camera.forward.getY();
    view(2, 2) = -camera.forward.getZ();
    view(2, 3) = Math::VectorCalc::dot(camera.forward, cameraEye);

    view(3, 3) = 1.0f;

    return view;
}

Math::Matrix44f ViewportCamera::viewProjection(s32 viewportWidth, s32 viewportHeight) const {
    const f32 aspect = (viewportHeight > 0) ? static_cast<f32>(viewportWidth) / static_cast<f32>(viewportHeight) : 1.0f;
    auto projection = makePerspective(cVerticalFov, aspect, cNear, cFar);

    projection(0, 0) *= mZoom;
    projection(1, 1) *= mZoom;

    return Math::MatrixCalc::multiply(projection, makeView());
}

ViewportCamera::Basis ViewportCamera::basis() const {
    const f32 cosYaw = std::cos(mYaw);
    const f32 sinYaw = std::sin(mYaw);

    const f32 cosPitch = std::cos(mPitch);
    const f32 sinPitch = std::sin(mPitch);

    const Math::Vector3f forward{
        -cosPitch * sinYaw,
        -sinPitch,
        -cosPitch * cosYaw
    };

    const Math::Vector3f right{
        cosYaw,
        0.0f,
        -sinYaw
    };

    const auto up = Math::VectorCalc::cross(right, forward);

    return {
        right,
        up,
        forward
    };
}

void ViewportCamera::orbit(QPoint delta) {
    mYaw -= static_cast<f32>(delta.x()) * 0.01f;
    mPitch = qBound(-1.55f, mPitch + static_cast<f32>(delta.y()) * 0.01f, 1.55f);
}

void ViewportCamera::pan(QPoint delta, s32 viewportHeight) {
    if (viewportHeight <= 0) {
        return;
    }

    const f32 visibleHalfHeight = std::tan(cVerticalFov * 0.5f) * cCameraDistance / mZoom;
    const f32 worldPerPixel = (2.0f * visibleHalfHeight) / static_cast<f32>(viewportHeight);

    const auto camera = basis();
    const f32 deltaX = -static_cast<f32>(delta.x()) * worldPerPixel;
    const f32 deltaY =  static_cast<f32>(delta.y()) * worldPerPixel;

    const Math::Vector3f panDelta = Math::VectorCalc::add(
        Math::VectorCalc::multScalar(camera.right, deltaX),
        Math::VectorCalc::multScalar(camera.up, deltaY)
    );

    mTarget = Math::VectorCalc::add(mTarget, panDelta);
}

void ViewportCamera::zoom(f32 delta) {
    mZoom += delta * std::pow(mZoom, cZoomExponent);
    mZoom = qBound(cZoomMin, mZoom, cZoomMax);
}

f32 ViewportCamera::zoomFactor() const {
    return mZoom;
}

const Math::Vector3f& ViewportCamera::target() const {
    return mTarget;
}


// ========================================================================== //


} // namespace PtclTool
