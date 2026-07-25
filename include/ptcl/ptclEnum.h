#pragma once

#include "typedefs.h"

#include <QString>


namespace Ptcl {


// ========================================================================== //


template<typename EnumType>
QString toString(const EnumType&) {
    static_assert(sizeof(EnumType) == 0, "toString not implemented for this enum type");
    return {};
}


// ========================================================================== //


enum class BillboardType : u32 {
    Billboard       = 0x0,
    PolygonXY       = 0x1,
    PolygonXZ       = 0x2,
    VelLook         = 0x3,
    VelLookPolygon  = 0x4,
    Stripe          = 0x5,
    ComplexStripe   = 0x6,
    Primitive       = 0x7,
    BillboardY      = 0x8,
};


// ========================================================================== //


enum class EmitterType : u32 {
    Simple     = 0x0,
    Complex    = 0x1,
    Compact    = 0x2,
};


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
};


// ========================================================================== //


enum class FollowType : u32 {
    All         = 0x0,
    None        = 0x1,
    PosOnly     = 0x2,
};


// ========================================================================== //


enum class RotType : u32 {
    None    = 0x0,
    RotX    = 0x1,
    RotY    = 0x2,
    RotZ    = 0x3,
    RotXYZ  = 0x4,
};


// ========================================================================== //


enum class ColorCalcType : u8 {
    None        = 0x0,
    Pass1       = 0x1,
    Interpolate = 0x2,
};


// ========================================================================== //


enum class BlendFuncType : u32 {
    Translucent     = 0x0,
    Additive        = 0x1,
    Subtractive     = 0x2,
    Transparence    = 0x3,
};


// ========================================================================== //


enum class DepthFuncType : u32 {
    Unk0    = 0x0,
    Unk1    = 0x1,
    Unk2    = 0x2,
};


// ========================================================================== //


enum class ColorCombinerFuncType : u32 {
    CombinerConfig0     = 0x0,
    CombinerConfig1     = 0x1,
    CombinerConfig2     = 0x2,
    CombinerConfig3     = 0x3,
    CombinerConfig4     = 0x4,
    CombinerConfig5     = 0x5,
    CombinerConfig6     = 0x6,
    CombinerConfig7     = 0x7,
    CombinerConfig8     = 0x8,
    CombinerConfig9     = 0x9,
    CombinerConfig10    = 0xA,
    CombinerConfig11    = 0xB,
    CombinerConfig12    = 0xC,
    CombinerConfig13    = 0xD,
    CombinerConfig14    = 0xE,
    CombinerConfig15    = 0xF,
    CombinerConfig16    = 0x10,
    CombinerConfig17    = 0x11,
    CombinerConfig18    = 0x12,
    CombinerConfig19    = 0x13,
    CombinerConfig20    = 0x14,
    CombinerConfig21    = 0x15,
    CombinerConfig22    = 0x16,
    CombinerConfig23    = 0x17,
    CombinerConfig24    = 0x18,
    CombinerConfig25    = 0x19,
    CombinerConfig26    = 0x1A,
    CombinerConfig27    = 0x1B,
};


// ========================================================================== //


enum class TextureFormat : u16 {
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

template<>
inline QString toString<TextureFormat>(const TextureFormat& type) {
    switch (type) {
    case TextureFormat::RGBA8888: return "RGBA8888";
    case TextureFormat::RGB888:   return "RGB888";
    case TextureFormat::RGBA5551: return "RGBA5551";
    case TextureFormat::RGB565:   return "RGB565";
    case TextureFormat::RGBA4444: return "RGBA4444";
    case TextureFormat::LA88:     return "LA88";
    case TextureFormat::HL8:      return "HL8";
    case TextureFormat::L8:       return "L8";
    case TextureFormat::A8:       return "A8";
    case TextureFormat::LA44:     return "LA44";
    case TextureFormat::L4:       return "L4";
    case TextureFormat::A4:       return "A4";
    case TextureFormat::ETC1:     return "ETC1";
    case TextureFormat::ETC1_A4:  return "ETC1_A4";
    default: std::unreachable();
    }
}

inline TextureFormat& assignFromInt(TextureFormat& type, int value) {
    type = static_cast<TextureFormat>(value);
    return type;
}


// ========================================================================== //


enum class TextureWrap : u8 {
    MirroredRepeat   = 0x0,
    Repeat           = 0x1,
    ClampToEdge      = 0x2,
};

template<>
inline QString toString<TextureWrap>(const TextureWrap& type) {
    switch (type) {
    case TextureWrap::MirroredRepeat: return "Mirrored repeat";
    case TextureWrap::Repeat:         return "Repeat";
    case TextureWrap::ClampToEdge:    return "Clamp to edge";
    default: std::unreachable();
    }
}

inline TextureWrap& assignFromInt(TextureWrap& type, int value) {
    type = static_cast<TextureWrap>(value);
    return type;
}


// ========================================================================== //


enum class TextureFilter : u8 {
    Linear  = 0x0,
    Nearest = 0x1,
};

template<>
inline QString toString<TextureFilter>(const TextureFilter& type) {
    switch (type) {
    case TextureFilter::Linear:  return "Linear";
    case TextureFilter::Nearest: return "Nearest";
    default: std::unreachable();
    }
}

inline TextureFilter& assignFromInt(TextureFilter& type, int value) {
    type = static_cast<TextureFilter>(value);
    return type;
}


// ========================================================================== //


enum class EmitterFlag : u32 {
    ColorRandom     = 1 << 0,
    EnableFog       = 1 << 7,
    ColorAnimation  = 1 << 8,
};


// ========================================================================== //


enum class ChildFlag : u16 {
    Enabled                 = 1 << 0,
    Color0Inherit           = 1 << 1,
    AlphaInherit            = 1 << 2,
    ScaleInherit            = 1 << 3,
    RotateInherit           = 1 << 4,
    VelInherit              = 1 << 5,
    IsFollow                = 1 << 6,
    Unk80                   = 1 << 7,
    Unk100                  = 1 << 8,
    IsPolygon               = 1 << 9,
    IsEmitterBillboardMtx   = 1 << 10,
    ParentField             = 1 << 11,
    PreChildDraw            = 1 << 12,
};


// ========================================================================== //


enum class FieldFlag : u16 {
    Random      = 1 << 0,
    Magnet      = 1 << 1,
    Spin        = 1 << 2,
    Collision   = 1 << 3,
    Convergence = 1 << 4,
    PosAdd      = 1 << 5,

    Enabled = Random | Magnet | Spin | Collision | Convergence | PosAdd
};


// ========================================================================== //


enum class FluctuationFlag : u16 {
    Enabled     = 1 << 0,
    ApplyAlpha  = 1 << 1,
    ApplyScale  = 1 << 2,
};


// ========================================================================== //


enum class StripeFlag : u16 {
    EmitterCoord    = 1 << 0,
};


// ========================================================================== //


enum class FieldMagnetFlag : u32 {
    AxisTargetX = 1 << 0,
    AxisTargetY = 1 << 1,
    AxisTargetZ = 1 << 2,
};


// ========================================================================== //

enum class FieldSpinAxis : u32 {
    AxisX   = 0x0,
    AxisY   = 0x1,
    AxisZ   = 0x2,
};

inline FieldSpinAxis& assignFromInt(FieldSpinAxis& type, int value) {
    type = static_cast<FieldSpinAxis>(value);
    return type;
}


// ========================================================================== //


enum class FieldCollisionType : u16 {
    Die     = 0x0,
    Bounce  = 0x1,
};

inline FieldCollisionType& assignFromInt(FieldCollisionType& type, int value) {
    type = static_cast<FieldCollisionType>(value);
    return type;
}


// ========================================================================== //


enum class FieldConvergenceType : u32 {
    AssignedPos = 0x0,
    EmitterPos  = 0x1,
};

inline FieldConvergenceType& assignFromInt(FieldConvergenceType& type, int value) {
    type = static_cast<FieldConvergenceType>(value);
    return type;
}


// ========================================================================== //


enum class StripeType : u32 {
    Billboard       = 0x0,
    EmitterMatrix   = 0x1,
    EmitterUpDown   = 0x2,
};

inline StripeType& assignFromInt(StripeType& type, int value) {
    type = static_cast<StripeType>(value);
    return type;
}


// ========================================================================== //


} // namespace Ptcl
