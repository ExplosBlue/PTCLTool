#ifndef PTCLENUM_H
#define PTCLENUM_H

#include <QDebug>

#include "typedefs.h"

namespace Ptcl {


// ========================================================================== //


enum class EmitterType : u32 {

    Simple     = 0x0,
    Complex    = 0x1,
    UnkType2   = 0x2,

    EMITTER_TYPE_MAX
};

QDebug operator<<(QDebug dbg, const EmitterType& type);

const QString toString(const EmitterType& type);


// ========================================================================== //


enum class VolumeType : u32 {

    Point          = 0x0,

    Circle         = 0x1,
    CircleSameDiv  = 0x2,
    CircleFill     = 0x3,

    Sphere         = 0x4,
    SphereSameDiv  = 0x5,
    SphereFill     = 0x6,

    Cylinder       = 0x7,
    CylinderFill   = 0x8,

    Box            = 0x9,
    BoxFill        = 0xA,

    Line           = 0xB,
    LineSameDiv    = 0xC,

    Rectangle      = 0xD,

    VOLUME_TYPE_MAX
};

QDebug operator<<(QDebug dbg, const VolumeType& type);

QString toString(const VolumeType& type);


// ========================================================================== //


enum class TextureFormat : u8 {

    RGBA8888   = 0x0,
    RGB888     = 0x1,
    RGBA5551   = 0x2,
    RGB565     = 0x3,
    RGBA4444   = 0x4,
    LA88       = 0x5,
    HL8        = 0x6,
    L8         = 0x7,
    A8         = 0x8,
    LA44       = 0x9,
    L4         = 0xA,
    A4         = 0xB,
    ETC1       = 0xC,
    ETC1_A4    = 0xD,
};

TextureFormat& assignFromInt(TextureFormat& e, int value);

QDebug operator<<(QDebug dbg, const TextureFormat& type);

QString toQString(TextureFormat type);


// ========================================================================== //


enum class TextureWrap : u8 {

    ClampToEdge      = 0x0,
    ClampToBorder    = 0x1,
    Repeat           = 0x2,
    MirroredRepeat   = 0x3,
};

TextureWrap& assignFromInt(TextureWrap& e, int value);

QDebug operator<<(QDebug dbg, const TextureWrap& type);

const QString toString(const TextureWrap& type);


// ========================================================================== //


enum class TextureMagFilter : u8 {

    Nearest   = 0x0,
    Linear    = 0x1,
};

TextureMagFilter& assignFromInt(TextureMagFilter& e, int value);

QDebug operator<<(QDebug dbg, const TextureMagFilter& type);

const QString toString(const TextureMagFilter& type);


// ========================================================================== //


enum class TextureMinFilter : u8 {

    Nearest                 = 0x0,
    NearestMipmapNearest    = 0x1,
    NearestMipmapLinear     = 0x2,
    Linear                  = 0x3,
    LinearMipmapNearest     = 0x4,
    LinearMipmapLinear      = 0x5,
};

TextureMinFilter& assignFromInt(TextureMinFilter& e, int value);

QDebug operator<<(QDebug dbg, const TextureMinFilter& type);

const QString toString(const TextureMinFilter& type);


// ========================================================================== //


} // namespace Ptcl

#endif // PTCLENUM_H
