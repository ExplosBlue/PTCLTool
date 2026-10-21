#pragma once

#include "gfx/camera.h"
#include "math/vector.h"
#include "ptcl/renderer/drawFrame.h"


namespace Ptcl::Render {


// ========================================================================== //

struct ParticleAxes {
    Math::Vector3f x;
    Math::Vector3f y;

    Math::Vector3f center(const Math::Vector3f& position, const Math::Vector2f& pivot) const;
};


struct StripeEdges {
    Math::Vector3f left;
    Math::Vector3f right;
};


// ========================================================================== //


ParticleAxes computeParticleAxes(const EmitterState& state, const ParticleState& particle, const Gfx::Camera& camera);
StripeEdges computeStripeEdges(const StripePoint& point, const Gfx::Camera& camera);


// ========================================================================== //


} // namespace Ptcl::Render
