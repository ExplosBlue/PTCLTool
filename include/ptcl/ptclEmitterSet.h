#pragma once

#include "ptcl/ptclEmitter.h"
#include "typedefs.h"

#include <QString>

#include <memory>
#include <vector>


namespace Ptcl {


// ========================================================================== //


class BinEmitterSet;

using EmitterList = std::vector<std::unique_ptr<Emitter>>;

class EmitterSet
{
public:
    EmitterSet() = default;
    EmitterSet(QString emitterName);
    EmitterSet(const BinEmitterSet& binEmitterSet);

    EmitterList& emitters();

    const QString& name() const;
    void setName(const QString& name);

    s32 emitterCount() const;

    u32 lastUpdateDate() const;
    void setLastUpdateDate(u32 lastUpdateDate);

    u32 userData() const;
    void setUserData(u32 data);

    void insertEmitter(s32 emitterIndex, std::unique_ptr<Emitter> emitter);
    std::unique_ptr<Emitter> removeEmitter(s32 emitterIndex);

    std::unique_ptr<EmitterSet> clone() const;

private:
    QString mName{};
    EmitterList mEmitters{};

    u32 mUserData{0};
    u32 mLastUpdateDate{0}; // TODO: check this
};


// ========================================================================== //


} // namespace Ptcl
