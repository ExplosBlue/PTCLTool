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

static Math::Matrix44f makeOrthographic(f32 aspect, f32 nearZ, f32 farZ) {
    const f32 halfHeight = std::tan(cVerticalFov * 0.5f) * cCameraDistance;
    const f32 halfWidth = halfHeight * aspect;
    const f32 depthScale = -2.0f / (farZ - nearZ);
    const f32 depthOffset = -(farZ + nearZ) / (farZ - nearZ);
    return {{
        {1.0f / halfWidth, 0.0f,              0.0f,       0.0f       },
        {0.0f,             1.0f / halfHeight, 0.0f,       0.0f       },
        {0.0f,             0.0f,              depthScale, depthOffset},
        {0.0f,             0.0f,              0.0f,       1.0f       },
    }};
}


// ========================================================================== //


Gfx::ViewBasis ViewportCamera::computeBasis() const {
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

    return {
        right,
        Math::VectorCalc::cross(right, forward),
        forward
    };
}

Math::Vector3f ViewportCamera::eye(const Gfx::ViewBasis& basis) const {
    return Math::VectorCalc::add(
        mTarget,
        Math::VectorCalc::multScalar(basis.forward, -cCameraDistance)
    );
}

Math::Matrix44f ViewportCamera::makeView(const Gfx::ViewBasis& basis) const {
    const Math::Vector3f cameraEye = eye(basis);

    Math::Matrix44f view{};
    view(0, 0) = basis.right.getX();
    view(0, 1) = basis.right.getY();
    view(0, 2) = basis.right.getZ();
    view(0, 3) = -Math::VectorCalc::dot(basis.right, cameraEye);

    view(1, 0) = basis.up.getX();
    view(1, 1) = basis.up.getY();
    view(1, 2) = basis.up.getZ();
    view(1, 3) = -Math::VectorCalc::dot(basis.up, cameraEye);

    view(2, 0) = -basis.forward.getX();
    view(2, 1) = -basis.forward.getY();
    view(2, 2) = -basis.forward.getZ();
    view(2, 3) = Math::VectorCalc::dot(basis.forward, cameraEye);

    view(3, 3) = 1.0f;

    return view;
}

Math::Matrix44f ViewportCamera::makeProjection() const {
    const f32 aspect = (mViewportHeight > 0)
        ? static_cast<f32>(mViewportWidth) / static_cast<f32>(mViewportHeight)
        : 1.0f;

    auto result = (mProjectionType == ProjectionType::Perspective)
        ? makePerspective(cVerticalFov, aspect, cNear, cFar)
        : makeOrthographic(aspect, cNear, cFar);

    result(0, 0) *= mZoom;
    result(1, 1) *= mZoom;
    return result;
}

void ViewportCamera::ensureCacheUpToDate() const {
    if (!mCacheDirty) {
        return;
    }

    mCachedBasis = computeBasis();
    mCachedViewProjection = Math::MatrixCalc::multiply(makeProjection(), makeView(mCachedBasis));
    mCacheDirty = false;
}

const Math::Matrix44f& ViewportCamera::viewProjection() const {
    ensureCacheUpToDate();
    return mCachedViewProjection;
}

const Gfx::ViewBasis& ViewportCamera::viewBasis() const {
    ensureCacheUpToDate();
    return mCachedBasis;
}

void ViewportCamera::setViewportSize(s32 width, s32 height) {
    if (mViewportWidth == width && mViewportHeight == height) {
        return;
    }

    mViewportWidth = width;
    mViewportHeight = height;
    invalidate();
}

void ViewportCamera::setProjectionType(ProjectionType type) {
    if (mProjectionType == type) {
        return;
    }

    mProjectionType = type;
    invalidate();
}

void ViewportCamera::orbit(QPoint delta) {
    mYaw -= static_cast<f32>(delta.x()) * 0.01f;
    mPitch = qBound(-1.55f, mPitch + static_cast<f32>(delta.y()) * 0.01f, 1.55f);
    invalidate();
}

void ViewportCamera::pan(QPoint delta) {
    if (mViewportHeight <= 0) {
        return;
    }

    const f32 visibleHalfHeight = std::tan(cVerticalFov * 0.5f) * cCameraDistance / mZoom;
    const f32 worldPerPixel = (2.0f * visibleHalfHeight) / static_cast<f32>(mViewportHeight);

    const Gfx::ViewBasis basis = computeBasis();
    const f32 deltaX = -static_cast<f32>(delta.x()) * worldPerPixel;
    const f32 deltaY =  static_cast<f32>(delta.y()) * worldPerPixel;

    const Math::Vector3f panDelta = Math::VectorCalc::add(
        Math::VectorCalc::multScalar(basis.right, deltaX),
        Math::VectorCalc::multScalar(basis.up, deltaY)
    );

    mTarget = Math::VectorCalc::add(mTarget, panDelta);
    invalidate();
}

void ViewportCamera::zoom(f32 delta) {
    mZoom += delta * std::pow(mZoom, cZoomExponent);
    mZoom = qBound(cZoomMin, mZoom, cZoomMax);
    invalidate();
}


// ========================================================================== //


} // namespace PtclTool
