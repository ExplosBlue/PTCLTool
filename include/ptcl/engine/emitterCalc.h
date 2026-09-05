#pragma once


namespace Ptcl::Engine {


// ========================================================================== //


class EmitterInstance;
class PtclInstance;

class EmitterCalc {
public:
    static void emitCommon(EmitterInstance& emitter, PtclInstance& ptcl);
    static void addParticle(EmitterInstance& emitter, PtclInstance& ptcl);
};


// ========================================================================== //


} // namespace Ptcl::Engine
