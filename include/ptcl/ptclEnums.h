#ifndef PTCLENUMS_H
#define PTCLENUMS_H

#include "../typedefs.h"

#include <QDebug>

namespace Ptcl {

enum class EmitterType : u32
{
    SIMPLE             = 0x0,
    COMPLEX            = 0x1,

    EMITTER_TYPE_MAX   = 0x2,
};

inline QDebug operator<<(QDebug dbg, const EmitterType& type)
{
    QDebugStateSaver stateSaver(dbg);

    switch (type) {
    case EmitterType::SIMPLE:
        dbg.nospace() << "SIMPLE";
        break;
    case EmitterType::COMPLEX:
        dbg.nospace() << "COMPLEX";
        break;
    case EmitterType::EMITTER_TYPE_MAX:
        dbg.nospace() << "EMITTER_TYPE_MAX";
        break;
    }

    return dbg;
}

enum class VolumeType : u32
{
    POINT                 = 0x0,
    CIRCLE                = 0x1,
    CIRCLE_SAME_DIVIDE    = 0x2,
    CIRCLE_FILL           = 0x3,
    SPHERE                = 0x4,
    SPHERE_SAME_DIVIDE    = 0x5,
    SPHERE_FILL           = 0x6,
    CYLINDER              = 0x7,
    CYLINDER_FILL         = 0x8,
    BOX                   = 0x9,
    BOX_FILL              = 0xA,
    LINE                  = 0xB,
    LINE_SAME_DIVIDE      = 0xC,
    RECTANGLE             = 0xD,

    VOLUME_TYPE_MAX       = 0xE,
};

inline QDebug operator<<(QDebug dbg, const VolumeType& type)
{
    QDebugStateSaver stateSaver(dbg);

    switch (type) {
    case VolumeType::POINT:
        dbg.nospace() << "POINT";
        break;
    case VolumeType::CIRCLE:
        dbg.nospace() << "CIRCLE";
        break;
    case VolumeType::CIRCLE_SAME_DIVIDE:
        dbg.nospace() << "CIRCLE_SAME_DIVIDE";
        break;
    case VolumeType::CIRCLE_FILL:
        dbg.nospace() << "CIRCLE_FILL";
        break;
    case VolumeType::SPHERE:
        dbg.nospace() << "SPHERE";
        break;
    case VolumeType::SPHERE_SAME_DIVIDE:
        dbg.nospace() << "SPHERE_SAME_DIVIDE";
        break;
    case VolumeType::SPHERE_FILL:
        dbg.nospace() << "SPHERE_FILL";
        break;
    case VolumeType::CYLINDER:
        dbg.nospace() << "CYLINDER";
        break;
    case VolumeType::CYLINDER_FILL:
        dbg.nospace() << "CYLINDER_FILL";
        break;
    case VolumeType::BOX:
        dbg.nospace() << "BOX";
        break;
    case VolumeType::BOX_FILL:
        dbg.nospace() << "BOX_FILL";
        break;
    case VolumeType::LINE:
        dbg.nospace() << "LINE";
        break;
    case VolumeType::LINE_SAME_DIVIDE:
        dbg.nospace() << "LINE_SAME_DIVIDE";
        break;
    case VolumeType::RECTANGLE:
        dbg.nospace() << "RECTANGLE";
        break;
    case VolumeType::VOLUME_TYPE_MAX:
        dbg.nospace() << "VOLUME_TYPE_MAX";
        break;
    }

    return dbg;
}

} // namespace Ptcl

#endif // PTCLENUMS_H
