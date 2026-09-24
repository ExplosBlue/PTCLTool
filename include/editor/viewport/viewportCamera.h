#pragma once

#include <QPoint>
#include <numbers>

#include "typedefs.h"
#include "math/matrix.h"
#include "math/vector.h"


namespace PtclTool {


// ========================================================================== //


class ViewportCamera {
public:
    enum class ProjectionType {
        Perspective,
        Orthographic,
    };

    struct Basis {
        Math::Vector3f right;
        Math::Vector3f up;
        Math::Vector3f forward;
    };

public:
    ViewportCamera();

    Math::Matrix44f viewProjection(s32 viewportWidth, s32 viewportHeight) const;
    Basis basis() const;

    ProjectionType projectionType() const;
    void setProjectionType(ProjectionType type);

    void orbit(QPoint delta);
    void pan(QPoint delta, s32 viewportHeight);
    void zoom(f32 delta);

    f32 zoomFactor() const;
    const Math::Vector3f& target() const;

private:
    Math::Vector3f eye() const;
    Math::Vector3f eye(const Basis& camera) const;
    Math::Matrix44f makeView() const;
    Math::Matrix44f makeProjection(s32 viewportWidth, s32 viewportHeight) const;

private:
    f32 mZoom{1.0f};
    ProjectionType mProjectionType{ProjectionType::Perspective};
    f32 mYaw{std::numbers::pi_v<f32>};
    f32 mPitch{0.0f};
    Math::Vector3f mTarget{};
};


// ========================================================================== //


} // namespace PtclTool
