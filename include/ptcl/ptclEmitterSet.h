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
    EmitterSet(const BinEmitterSet& binEmitterSet);

    EmitterList& emitters();

    const QString& name() const;
    void setName(const QString& name);

    s32 emitterCount() const;

    u32 lastUpdateDate() const;
    void setLastUpdateDate(u32 lastUpdateDate);

    u32 userData() const;
    void setUserData(u32 data);

    void addNewEmitter();
    void removeEmitter(s32 emitterIndex);

private:
    QString mName;
    EmitterList mEmitters;

    u32 mUserData;
    u32 mLastUpdateDate; // TODO: check this
};


// ========================================================================== //


} // namespace Ptcl
