#include "ptcl/ptclEmitterSet.h"

namespace Ptcl {

EmitterSet::EmitterSet(const BinEmitterSet& binEmitterSet) {

}

EmitterList& EmitterSet::emitters() {

    return mEmitters;
}

const QString& EmitterSet::name() const {

    return mName;
}

void EmitterSet::setName(const QString& name) {

    mName = name;
}

u32 EmitterSet::emitterCount() const {

    return mEmitters.size();
}

u32 EmitterSet::lastUpdateDate() const {

    return mLastUpdateDate;
}

void EmitterSet::setLastUpdateDate(u32 lastUpdateDate) {

    mLastUpdateDate = lastUpdateDate;
}

u32 EmitterSet::userData() const {

    return mUserData;
}

void EmitterSet::setUserData(u32 data) {

    mUserData = data;
}


} // namespace Ptcl
