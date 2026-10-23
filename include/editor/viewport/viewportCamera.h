#pragma once

#include <QPoint>
#include <numbers>

#include "gfx/camera.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "typedefs.h"


namespace PtclTool {


// ========================================================================== //


class ViewportCamera : public Gfx::Camera {
public:
    enum class ProjectionType {
        Perspective,
        Orthographic,
    };

    enum class ViewDirection {
        Front,
        Back,
        Left,
        Right,
        Top,
        Bottom,
    };

    struct Orientation {
        f32 yaw{0.0f};
        f32 pitch{0.0f};
    };

    const Math::Matrix44f& viewProjection() const override;
    const Gfx::ViewBasis& viewBasis() const override;

    const Math::Matrix44f& inverseViewProjection() const;
    Math::Vector3f position() const;
    f32 viewDistance() const;

    void setViewportSize(s32 width, s32 height);
    void setProjectionType(ProjectionType type);
    ProjectionType projectionType() const { return mProjectionType; }

    Orientation orientation() const { return {mYaw, mPitch}; }
    void setOrientation(const Orientation& orientation);
    static Orientation orientationFor(ViewDirection direction);

    void setViewDirection(ViewDirection direction);
    void reset();

    void orbit(QPoint delta, bool snap);
    void pan(QPoint delta);
    void zoom(f32 delta);

private:
    Gfx::ViewBasis computeBasis() const;
    Math::Vector3f eye(const Gfx::ViewBasis& basis) const;
    Math::Matrix44f makeView(const Gfx::ViewBasis& basis) const;
    Math::Matrix44f makeProjection() const;

    void ensureCacheUpToDate() const;
    void invalidate() const { mCacheDirty = true; }

    static constexpr f32 cDefaultYaw = std::numbers::pi_v<f32> * 0.25f;
    static constexpr f32 cDefaultPitch = std::numbers::pi_v<f32> / 6.0f;

    f32 mZoom{1.0f};
    ProjectionType mProjectionType{ProjectionType::Perspective};
    f32 mYaw{cDefaultYaw};
    f32 mPitch{cDefaultPitch};
    Math::Vector3f mTarget{};

    s32 mViewportWidth{1};
    s32 mViewportHeight{1};

    mutable Gfx::ViewBasis mCachedBasis{};
    mutable Math::Matrix44f mCachedViewProjection{};
    mutable Math::Matrix44f mCachedInverseViewProjection{};
    mutable bool mCacheDirty{true};
};


// ========================================================================== //


} // namespace PtclTool
