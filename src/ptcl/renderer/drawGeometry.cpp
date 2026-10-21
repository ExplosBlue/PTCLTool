#include "ptcl/renderer/drawGeometry.h"

#include <initializer_list>
#include <iterator>
#include <utility>

#include "math/matrix.h"


namespace Ptcl::Render {


// ========================================================================== //


namespace {

constexpr f32 cMinDirectionLength = 0.0001f;

constexpr Math::Vector3f kWorldUp{0.0f, 1.0f, 0.0f};
constexpr Math::Vector3f kAxisX{1.0f, 0.0f, 0.0f};


bool isUsable(const Math::Vector3f& v) {
    return v.length() > cMinDirectionLength;
}

Math::Vector3f chooseDirection(std::initializer_list<Math::Vector3f> candidates) {
    Math::Vector3f chosen = *std::rbegin(candidates);
    for (const Math::Vector3f& candidate : candidates) {
        if (isUsable(candidate)) {
            chosen = candidate;
            break;
        }
    }

    Math::VectorCalc::normalize(chosen);
    return chosen;
}

Math::Vector3f flattenOntoPlane(const Math::Vector3f& v, const Math::Vector3f& unitAxis) {
    return Math::VectorCalc::sub(
        v,
        Math::VectorCalc::multScalar(unitAxis, Math::VectorCalc::dot(v, unitAxis))
    );
}

Math::Vector3f toCameraSpace(const Gfx::ViewBasis& view, const Math::Vector3f& local) {
    const Math::Vector3f frameX = Math::VectorCalc::multScalar(view.right, -1.0f);

    Math::Vector3f result = Math::VectorCalc::multScalar(frameX, local.getX());

    result = Math::VectorCalc::add(
        result,
        Math::VectorCalc::multScalar(view.up, local.getY())
    );

    return Math::VectorCalc::add(
        result,
        Math::VectorCalc::multScalar(view.forward, local.getZ())
    );
}

enum class ParticleColumn : std::size_t {
    LocalX   = 1,
    LocalY   = 2,
    Position = 3,
};

Math::Vector3f particleAxis(const Math::Matrix34f& matrix, ParticleColumn column) {
    const auto index = std::to_underlying(column);
    return {
        matrix(0, index),
        matrix(1, index),
        matrix(2, index)
    };
}

struct ParticleFrame {
    const EmitterState& state;
    const ParticleState& particle;
    const Gfx::ViewBasis& view;

    const Math::Matrix34f& basis;

    Math::Vector3f localX;
    Math::Vector3f localY;
    Math::Vector3f position;

    Math::Vector3f worldPosition;
    Math::Vector3f worldY;

    f32 width;
    f32 height;
};

ParticleFrame makeParticleFrame(const EmitterState& state, const ParticleState& particle, const Gfx::Camera& camera) {
    const Math::Matrix34f& basis = (particle.followMode == ParticleFollowMode::Basis)
        ? particle.followBasis
        : state.emitterRT;

    const Math::Vector3f localX   = particleAxis(particle.matrix, ParticleColumn::LocalX);
    const Math::Vector3f localY   = particleAxis(particle.matrix, ParticleColumn::LocalY);
    const Math::Vector3f position = particleAxis(particle.matrix, ParticleColumn::Position);

    return {
        .state         = state,
        .particle      = particle,
        .view          = camera.viewBasis(),
        .basis         = basis,
        .localX        = localX,
        .localY        = localY,
        .position      = position,
        .worldPosition = Math::VectorCalc::rotate(basis, position),
        .worldY        = Math::VectorCalc::rotate(basis, localY),
        .width         = particle.scale.getX(),
        .height        = particle.scale.getY(),
    };
}

ParticleAxes emitterAlignedAxes(const ParticleFrame& frame) {
    return {
        Math::VectorCalc::multScalar(frame.worldPosition, frame.width),
        Math::VectorCalc::multScalar(frame.worldY, frame.height),
    };
}

ParticleAxes billboardAxes(const ParticleFrame& frame) {
    if (frame.state.isEmitterBillboardMtx) {
        return emitterAlignedAxes(frame);
    }

    const Math::Vector3f upDir     = toCameraSpace(frame.view, frame.localY);
    const Math::Vector3f offsetDir = toCameraSpace(frame.view, frame.position);

    const Math::Vector3f rightDir = chooseDirection({
        Math::VectorCalc::cross(upDir, frame.view.forward),
        offsetDir,
        Math::VectorCalc::multScalar(frame.view.right, -1.0f),
    });

    const f32 facing = flattenOntoPlane(offsetDir, frame.view.forward).length();

    return {
        Math::VectorCalc::multScalar(rightDir, frame.width * facing),
        Math::VectorCalc::multScalar(upDir, frame.height),
    };
}

ParticleAxes billboardYAxes(const ParticleFrame& frame) {
    Math::Vector3f rightDir = Math::VectorCalc::add(
        Math::VectorCalc::multScalar(frame.view.right, frame.position.getX()),
        Math::VectorCalc::multScalar(frame.view.up, frame.position.getY())
    );

    Math::VectorCalc::normalize(rightDir);

    return {
        Math::VectorCalc::multScalar(rightDir, frame.width),
        Math::VectorCalc::multScalar(kWorldUp, frame.height),
    };
}

Math::Vector3f twistRightAxis(const Math::Vector3f& dir, const Math::Vector3f& right, const Math::Vector3f& reference, const Math::Vector3f& rotated) {
    const Math::Vector3f referenceFlat = flattenOntoPlane(reference, dir);
    const Math::Vector3f rotatedFlat   = flattenOntoPlane(rotated, dir);

    if (!isUsable(referenceFlat) || !isUsable(rotatedFlat)) {
        return right;
    }

    const Math::Vector3f referenceDir = Math::VectorCalc::multScalar(referenceFlat, 1.0f / referenceFlat.length());
    const Math::Vector3f rotatedDir   = Math::VectorCalc::multScalar(rotatedFlat, 1.0f / rotatedFlat.length());

    const f32 cosine = Math::VectorCalc::dot(referenceDir, rotatedDir);
    const f32 sine   = Math::VectorCalc::dot(Math::VectorCalc::cross(referenceDir, rotatedDir), dir);

    const Math::Vector3f tangent = Math::VectorCalc::cross(dir, right);

    Math::Vector3f result = Math::VectorCalc::add(
        Math::VectorCalc::multScalar(right, cosine),
        Math::VectorCalc::multScalar(tangent, sine)
    );

    Math::VectorCalc::normalize(result);

    return result;
}

ParticleAxes velocityBillboardAxes(const ParticleFrame& frame) {
    const Math::Vector3f direction = chooseDirection({frame.particle.followDirection, frame.view.up});

    const f32 axisSign = (frame.state.rotationBasis.getY() < 0.0f) ? -1.0f : 1.0f;
    const Math::Vector3f axisY = Math::VectorCalc::multScalar(direction, axisSign * frame.height);

    const Math::Vector3f reference = (frame.state.billboardType == BillboardType::VelLookPolygon)
        ? Math::VectorCalc::rotate(frame.basis, kAxisX)
        : frame.view.forward;

    const Math::Vector3f baseRight = chooseDirection({
        Math::VectorCalc::cross(direction, reference),
        frame.view.right,
    });

    const Math::Vector3f right = twistRightAxis(direction, baseRight, kAxisX, frame.position);

    return {
        Math::VectorCalc::multScalar(right, frame.width),
        axisY,
    };
}


} // namespace


// ========================================================================== //


Math::Vector3f ParticleAxes::center(const Math::Vector3f& position, const Math::Vector2f& pivot) const {
    const Math::Vector3f pivotOffset = Math::VectorCalc::add(
        Math::VectorCalc::multScalar(x, pivot.getX()),
        Math::VectorCalc::multScalar(y, pivot.getY())
    );

    return Math::VectorCalc::add(position, pivotOffset);
}

ParticleAxes computeParticleAxes(const EmitterState& state, const ParticleState& particle, const Gfx::Camera& camera) {
    const ParticleFrame frame = makeParticleFrame(state, particle, camera);

    switch (state.billboardType) {
    case BillboardType::Billboard:
        return billboardAxes(frame);

    case BillboardType::BillboardY:
        return billboardYAxes(frame);

    case BillboardType::VelLook:
    case BillboardType::VelLookPolygon:
        return velocityBillboardAxes(frame);

    case BillboardType::PolygonXZ: {
        ParticleAxes axes = emitterAlignedAxes(frame);
        axes.y = Math::VectorCalc::multScalar(
            Math::VectorCalc::rotate(frame.basis, frame.localX), frame.height
        );
        return axes;
    }

    case BillboardType::PolygonXY:
    case BillboardType::Stripe:
    case BillboardType::ComplexStripe:
    case BillboardType::Primitive:
    default:
        return emitterAlignedAxes(frame);
    }
}

StripeEdges computeStripeEdges(const StripePoint& point, const Gfx::Camera& camera) {
    Math::Vector3f edge = point.outer;

    if (point.billboard) {
        const Gfx::ViewBasis& view = camera.viewBasis();
        const Math::Vector3f direction = chooseDirection({point.outer, view.up});
        const Math::Vector3f facing = chooseDirection({
            Math::VectorCalc::cross(direction, view.forward),
            view.right,
        });
        edge = Math::VectorCalc::multScalar(facing, point.scale * 0.5f);
    }

    return {
        Math::VectorCalc::add(point.position, edge),
        Math::VectorCalc::add(point.position, Math::VectorCalc::multScalar(edge, -1.0f)),
    };
}


// ========================================================================== //


} // namespace Ptcl::Render
