#include "ptcl/ptclCommand.h"
#include "ptcl/ptclDocument.h"

namespace Ptcl {


// ========================================================================== //

template <typename T>
Emitter& SetEmitterPropertyCommand<T>::getEmitter() const {
    return *mDocument->emitter(mSetIndex, mEmitterIndex);
}

template <typename T>
void SetEmitterPropertyCommand<T>::apply(const T& value) {
    auto& emitter = getEmitter();
    mSetter(emitter, value);
    mDocument->notifyEmitterChanged(mSetIndex, mEmitterIndex);
}


// ========================================================================== //


} // namespace Ptcl
