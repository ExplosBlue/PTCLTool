#include "ptcl/ptclEmitterSet.h"


namespace Ptcl {


// ========================================================================== //


EmitterSet::EmitterSet(const BinEmitterSet& binEmitterSet) {}

EmitterList& EmitterSet::emitters() {
    return mEmitters;
}

const QString& EmitterSet::name() const {
    return mName;
}

void EmitterSet::setName(const QString& name) {
    mName = name;
}

s32 EmitterSet::emitterCount() const {
    return static_cast<s32>(mEmitters.size());
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

void EmitterSet::addNewEmitter() {
    auto newEmitter = std::make_unique<Ptcl::Emitter>();
    newEmitter->setName("New_Emitter_" + QString::number(emitterCount()));
    mEmitters.push_back(std::move(newEmitter));
}

void EmitterSet::removeEmitter(s32 emitterIndex) {
    if (emitterIndex >= mEmitters.size()) {
        return;
    }

    mEmitters.erase(mEmitters.begin() + emitterIndex);
}

std::unique_ptr<EmitterSet> EmitterSet::clone() const {
    auto newSet = std::make_unique<EmitterSet>();

    newSet->mName = mName;
    newSet->mUserData = mUserData;
    newSet->mLastUpdateDate = mLastUpdateDate;

    for (const auto& emitter : mEmitters) {
        if (emitter) {
            newSet->mEmitters.push_back(emitter->clone());
        }
    }
    return newSet;
}

const std::unique_ptr<Emitter>& EmitterSet::appendEmitter(std::unique_ptr<Emitter>& newSet) {
    mEmitters.push_back(std::move(newSet));
    return mEmitters.at(emitterCount() - 1);
}


// ========================================================================== //


} // namespace Ptcl
