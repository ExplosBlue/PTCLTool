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

    const Math::Matrix44f& viewProjection() const override;
    const Gfx::ViewBasis& viewBasis() const override;

    void setViewportSize(s32 width, s32 height);
    void setProjectionType(ProjectionType type);

    void orbit(QPoint delta);
    void pan(QPoint delta);
    void zoom(f32 delta);

private:
    Gfx::ViewBasis computeBasis() const;
    Math::Vector3f eye(const Gfx::ViewBasis& basis) const;
    Math::Matrix44f makeView(const Gfx::ViewBasis& basis) const;
    Math::Matrix44f makeProjection() const;

    void ensureCacheUpToDate() const;
    void invalidate() const { mCacheDirty = true; }

    f32 mZoom{1.0f};
    ProjectionType mProjectionType{ProjectionType::Perspective};
    f32 mYaw{std::numbers::pi_v<f32>};
    f32 mPitch{0.0f};
    Math::Vector3f mTarget{};

    s32 mViewportWidth{1};
    s32 mViewportHeight{1};

    mutable Gfx::ViewBasis mCachedBasis{};
    mutable Math::Matrix44f mCachedViewProjection{};
    mutable bool mCacheDirty{true};
};


// ========================================================================== //


} // namespace PtclTool
