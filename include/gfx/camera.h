#pragma once

#include "math/matrix.h"
#include "math/vector.h"


namespace Gfx {


// ========================================================================== //


struct ViewBasis {
    Math::Vector3f right;
    Math::Vector3f up;
    Math::Vector3f forward;
};


// ========================================================================== //


class Camera {
public:
    virtual ~Camera() = default;

    virtual const Math::Matrix44f& viewProjection() const = 0;
    virtual const ViewBasis& viewBasis() const = 0;
};


// ========================================================================== //


} // namespace Gfx
