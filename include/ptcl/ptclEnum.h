#pragma once

#include "typedefs.h"

#include <QDebug>


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

    MAX
};

template<>
inline QString toString<BillboardType>(const BillboardType& type) {
    switch (type) {
    case BillboardType::Billboard:          return "Billboard";
    case BillboardType::PolygonXY:          return "XY-plane polygon";
    case BillboardType::PolygonXZ:          return "XZ-plane polygon";
    case BillboardType::VelLook:            return "Directional Y-billboard";
    case BillboardType::VelLookPolygon:     return "Directional polygon";
    case BillboardType::Stripe:             return "Stripe";
    case BillboardType::ComplexStripe:      return "ComplexStripe";
    case BillboardType::Primitive:          return "Primitive";
    case BillboardType::BillboardY:         return "Y-Axis Billboard";
    case BillboardType::MAX:                return "BILLBOARD_TYPE_MAX";
    default: return "UNKNOWN";
    }
}

inline QDebug operator<<(QDebug dbg, const BillboardType& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


// ========================================================================== //


enum class EmitterType : u32 {
    Simple     = 0x0,
    Complex    = 0x1,
    UnkType2   = 0x2,

    MAX
};

template<>
inline QString toString<EmitterType>(const EmitterType& type) {
    switch (type) {
    case EmitterType::Simple:           return "Simple";
    case EmitterType::Complex:          return "Complex";
    case EmitterType::UnkType2:         return "UnkType2";
    case EmitterType::MAX:              return "EMITTER_TYPE_MAX";
    default: return "UNKNOWN";
    }
}

inline QDebug operator<<(QDebug dbg, const EmitterType& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


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

    MAX
};

template<>
inline QString toString<VolumeType>(const VolumeType& type) {
    switch (type) {
    case VolumeType::Point:           return "Point";
    case VolumeType::Circle:          return "Circle";
    case VolumeType::CircleSameDiv:   return "CircleSameDiv";
    case VolumeType::CircleFill:      return "CircleFill";
    case VolumeType::Sphere:          return "Sphere";
    case VolumeType::SphereSameDiv:   return "SphereSameDiv";
    case VolumeType::SphereFill:      return "SphereFill";
    case VolumeType::Cylinder:        return "Cylinder";
    case VolumeType::CylinderFill:    return "CylinderFill";
    case VolumeType::Box:             return "Box";
    case VolumeType::BoxFill:         return "BoxFill";
    case VolumeType::Line:            return "Line";
    case VolumeType::LineSameDiv:     return "LineSameDiv";
    case VolumeType::Rectangle:       return "Rectangle";
    case VolumeType::MAX:             return "VOLUME_TYPE_MAX";
    default: return "UNKNOWN";
    }
}

inline QDebug operator<<(QDebug dbg, const VolumeType& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


// ========================================================================== //


enum class FollowType : u32 {
    All         = 0x0,
    None        = 0x1,
    PosOnly     = 0x2,

    MAX
};

template<>
inline QString toString<FollowType>(const FollowType& type) {
    switch (type) {
    case FollowType::All:       return "All";
    case FollowType::None:      return "None";
    case FollowType::PosOnly:   return "PosOnly";
    case FollowType::MAX:       return "FOLLOW_TYPE_MAX";
    default: return "UNKNOWN";
    }
}

inline QDebug operator<<(QDebug dbg, const FollowType& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


// ========================================================================== //


enum class RotType : u32 {
    None    = 0x0,
    RotX    = 0x1,
    RotY    = 0x2,
    RotZ    = 0x3,
    RotXYZ  = 0x4,

    MAX
};

template<>
inline QString toString<RotType>(const RotType& type) {
    switch (type) {
    case RotType::None:     return "None";
    case RotType::RotX:     return "RotX";
    case RotType::RotY:     return "RotY";
    case RotType::RotZ:     return "RotZ";
    case RotType::RotXYZ:   return "RotXYZ";
    case RotType::MAX:      return "ROT_TYPE_MAX";
    default: return "UNKNOWN";
    }
}

inline QDebug operator<<(QDebug dbg, const RotType& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


// ========================================================================== //


enum class ColorCalcType : u8 {
    None        = 0x0,
    Pass1       = 0x1,
    Interpolate = 0x2,

    MAX
};

template<>
inline QString toString<ColorCalcType>(const ColorCalcType& type) {
    switch (type) {
    case ColorCalcType::None:        return "None";
    case ColorCalcType::Pass1:       return "Pass1";
    case ColorCalcType::Interpolate: return "Interpolate";
    case ColorCalcType::MAX:         return "COMBINER_TYPE_MAX";
    default: return "UNKNOWN";
    }
}

inline QDebug operator<<(QDebug dbg, const ColorCalcType& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


// ========================================================================== //


enum class BlendFuncType : u32 {
    Translucent     = 0x0,
    Additive        = 0x1,
    Subtractive     = 0x2,
    Transparence    = 0x3,

    MAX
};

template<>
inline QString toString<BlendFuncType>(const BlendFuncType& type) {
    switch (type) {
    case BlendFuncType::Translucent:    return "Translucent";
    case BlendFuncType::Additive:       return "Additive";
    case BlendFuncType::Subtractive:    return "Subtractive";
    case BlendFuncType::Transparence:   return "Transparence";
    case BlendFuncType::MAX:            return "BLEND_FUNC_TYPE_MAX";
    default: return "UNKNOWN";
    }
}

inline QDebug operator<<(QDebug dbg, const BlendFuncType& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


// ========================================================================== //


enum class DepthFuncType : u32 {
    Unk0    = 0x0,
    Unk1    = 0x1,
    Unk2    = 0x2,

    MAX
};

template<>
inline QString toString<DepthFuncType>(const DepthFuncType& type) {
    switch (type) {
    case DepthFuncType::Unk0:   return "Unk0";
    case DepthFuncType::Unk1:   return "Unk1";
    case DepthFuncType::Unk2:   return "Unk2";
    case DepthFuncType::MAX:    return "DEPTH_FUNC_TYPE_MAX";
    default: return "UNKNOWN";
    }
}

inline QDebug operator<<(QDebug dbg, const DepthFuncType& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


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

    MAX
};

template<>
inline QString toString<ColorCombinerFuncType>(const ColorCombinerFuncType& type) {
    switch (type) {
    case ColorCombinerFuncType::CombinerConfig0:    return "Combiner Config 0";
    case ColorCombinerFuncType::CombinerConfig1:    return "Combiner Config 1";
    case ColorCombinerFuncType::CombinerConfig2:    return "Combiner Config 2";
    case ColorCombinerFuncType::CombinerConfig3:    return "Combiner Config 3";
    case ColorCombinerFuncType::CombinerConfig4:    return "Combiner Config 4";
    case ColorCombinerFuncType::CombinerConfig5:    return "Combiner Config 5";
    case ColorCombinerFuncType::CombinerConfig6:    return "Combiner Config 6";
    case ColorCombinerFuncType::CombinerConfig7:    return "Combiner Config 7";
    case ColorCombinerFuncType::CombinerConfig8:    return "Combiner Config 8";
    case ColorCombinerFuncType::CombinerConfig9:    return "Combiner Config 9";
    case ColorCombinerFuncType::CombinerConfig10:   return "Combiner Config 10";
    case ColorCombinerFuncType::CombinerConfig11:   return "Combiner Config 11";
    case ColorCombinerFuncType::CombinerConfig12:   return "Combiner Config 12";
    case ColorCombinerFuncType::CombinerConfig13:   return "Combiner Config 13";
    case ColorCombinerFuncType::CombinerConfig14:   return "Combiner Config 14";
    case ColorCombinerFuncType::CombinerConfig15:   return "Combiner Config 15";
    case ColorCombinerFuncType::CombinerConfig16:   return "Combiner Config 16";
    case ColorCombinerFuncType::CombinerConfig17:   return "Combiner Config 17";
    case ColorCombinerFuncType::CombinerConfig18:   return "Combiner Config 18";
    case ColorCombinerFuncType::CombinerConfig19:   return "Combiner Config 19";
    case ColorCombinerFuncType::CombinerConfig20:   return "Combiner Config 20";
    case ColorCombinerFuncType::CombinerConfig21:   return "Combiner Config 21";
    case ColorCombinerFuncType::CombinerConfig22:   return "Combiner Config 22";
    case ColorCombinerFuncType::CombinerConfig23:   return "Combiner Config 23";
    case ColorCombinerFuncType::CombinerConfig24:   return "Combiner Config 24";
    case ColorCombinerFuncType::CombinerConfig25:   return "Combiner Config 25";
    case ColorCombinerFuncType::CombinerConfig26:   return "Combiner Config 26";
    case ColorCombinerFuncType::CombinerConfig27:   return "Combiner Config 27";
    case ColorCombinerFuncType::MAX:    return "COLOR_COMBINER_FUNC_TYPE_MAX";
    default: return "UNKNOWN";
    }
}

inline QDebug operator<<(QDebug dbg, const ColorCombinerFuncType& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


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

    MAX
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
    default: return "Unknown";
    }
}

inline TextureFormat& assignFromInt(TextureFormat& type, int value) {
    type = static_cast<TextureFormat>(value);
    return type;
}

inline QDebug operator<<(QDebug dbg, const TextureFormat& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


// ========================================================================== //


enum class TextureWrap : u8 {
    ClampToEdge      = 0x0,
    ClampToBorder    = 0x1,
    Repeat           = 0x2,
    MirroredRepeat   = 0x3,

    MAX
};

template<>
inline QString toString<TextureWrap>(const TextureWrap& type) {
    switch (type) {
    case TextureWrap::ClampToEdge:    return "ClampToEdge";
    case TextureWrap::ClampToBorder:  return "ClampToBorder";
    case TextureWrap::Repeat:         return "Repeat";
    case TextureWrap::MirroredRepeat: return "MirroredRepeat";
    default: return "Unknown";
    }
}

inline TextureWrap& assignFromInt(TextureWrap& type, int value) {
    type = static_cast<TextureWrap>(value);
    return type;
}

inline QDebug operator<<(QDebug dbg, const TextureWrap& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


// ========================================================================== //


enum class TextureFilter : u8 {
    Nearest   = 0x0,
    Linear    = 0x1,

    MAX
};

template<>
inline QString toString<TextureFilter>(const TextureFilter& type) {
    switch (type) {
    case TextureFilter::Nearest: return "Nearest";
    case TextureFilter::Linear:  return "Linear";
    default: return "Unknown";
    }
}

inline TextureFilter& assignFromInt(TextureFilter& type, int value) {
    type = static_cast<TextureFilter>(value);
    return type;
}

inline QDebug operator<<(QDebug dbg, const TextureFilter& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


// ========================================================================== //


enum class TextureMipFilter : u8 {
    None    = 0x0,
    Nearest = 0x1,
    Linear  = 0x2,

    MAX
};

template<>
inline QString toString<TextureMipFilter>(const TextureMipFilter& type) {
    switch (type) {
    case TextureMipFilter::None:    return "None";
    case TextureMipFilter::Nearest: return "Nearest";
    case TextureMipFilter::Linear:  return "Linear";
    default: return "Unknown";
    }
}

inline TextureMipFilter& assignFromInt(TextureMipFilter& type, int value) {
    type = static_cast<TextureMipFilter>(value);
    return type;
}

inline QDebug operator<<(QDebug dbg, const TextureMipFilter& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
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
};


// ========================================================================== //


enum class FluctuationFlag : u16 {
    Enabled     = 1 << 0,
    ApplyAlpha  = 1 << 1,
    ApplyScale  = 1 << 2,
};


// ========================================================================== //


enum class StripeFlag : u16 {
    Unknown     = 1 << 0, // This is a placeholder
};


// ========================================================================== //


enum class FieldConvergenceType : u32 {
    AssignedPos = 0x1,
    EmitterPos  = 0x2
};

template<>
inline QString toString<FieldConvergenceType>(const FieldConvergenceType& type) {
    switch (type) {
    case FieldConvergenceType::AssignedPos: return "AssignedPos";
    case FieldConvergenceType::EmitterPos:  return "EmitterPos";
    default: return "Unknown";
    }
}

inline FieldConvergenceType& assignFromInt(FieldConvergenceType& type, int value) {
    type = static_cast<FieldConvergenceType>(value);
    return type;
}

inline QDebug operator<<(QDebug dbg, const FieldConvergenceType& type) {
    QDebugStateSaver stateSaver(dbg);
    dbg.nospace() << toString(type);
    return dbg;
}


// ========================================================================== //


} // namespace Ptcl
