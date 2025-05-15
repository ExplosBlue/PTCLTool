#include "ptcl/ptclEnum.h"

namespace Ptcl {


// ========================================================================== //


QDebug operator<<(QDebug dbg, const BillboardType& type) {

    QDebugStateSaver stateSaver(dbg);

    switch (type) {
    case BillboardType::Billboard:
        dbg.nospace() << "Billboard";
        break;
    case BillboardType::PolygonXY:
        dbg.nospace() << "PolygonXY";
        break;
    case BillboardType::PolygonXZ:
        dbg.nospace() << "PolygonXZ";
        break;
    case BillboardType::VelLook:
        dbg.nospace() << "VelLook";
        break;
    case BillboardType::VelLookPolygon:
        dbg.nospace() << "VelLookPolygon";
        break;
    case BillboardType::Stripe:
        dbg.nospace() << "Stripe";
        break;
    case BillboardType::ComplexStripe:
        dbg.nospace() << "ComplexStripe";
        break;
    case BillboardType::Primitive:
        dbg.nospace() << "Primitive";
        break;
    case BillboardType::BillboardY:
        dbg.nospace() << "BillboardY";
        break;
    case BillboardType::BILLBOARD_TYPE_MAX:
        dbg.nospace() << "BILLBOARD_TYPE_MAX";
        break;
    default:
        dbg.nospace() << "Unknown";
        break;
    }

    return dbg;
}

const QString toString(const BillboardType& type) {

    switch (type) {
    case BillboardType::Billboard:          return "Billboard";
    case BillboardType::PolygonXY:          return "PolygonXY";
    case BillboardType::PolygonXZ:          return "PolygonXZ";
    case BillboardType::VelLook:            return "VelLook";
    case BillboardType::VelLookPolygon:     return "VelLookPolygon";
    case BillboardType::Stripe:             return "Stripe";
    case BillboardType::ComplexStripe:      return "ComplexStripe";
    case BillboardType::Primitive:          return "Primitive";
    case BillboardType::BillboardY:         return "BillboardY";
    case BillboardType::BILLBOARD_TYPE_MAX: return "BILLBOARD_TYPE_MAX";
    default: return "UNKNOWN";
    }
}


// ========================================================================== //


QDebug operator<<(QDebug dbg, const EmitterType& type) {

    QDebugStateSaver stateSaver(dbg);

    switch (type) {
    case EmitterType::Simple:
        dbg.nospace() << "Simple";
        break;
    case EmitterType::Complex:
        dbg.nospace() << "Complex";
        break;
    case EmitterType::UnkType2:
        dbg.nospace() << "UnkType2";
        break;
    case EmitterType::EMITTER_TYPE_MAX:
        dbg.nospace() << "EMITTER_TYPE_MAX";
        break;
    default:
        dbg.nospace() << "Unknown";
        break;
    }

    return dbg;
}

const QString toString(const EmitterType& type) {

    switch (type) {
    case EmitterType::Simple:           return "Simple";
    case EmitterType::Complex:          return "Complex";
    case EmitterType::UnkType2:         return "UnkType2";
    case EmitterType::EMITTER_TYPE_MAX: return "EMITTER_TYPE_MAX";
    default: return "UNKNOWN";
    }
}


// ========================================================================== //


QDebug operator<<(QDebug dbg, const VolumeType& type) {

    QDebugStateSaver stateSaver(dbg);

    switch (type) {
    case VolumeType::Point:
        dbg.nospace() << "Point";
        break;
    case VolumeType::Circle:
        dbg.nospace() << "Circle";
        break;
    case VolumeType::CircleSameDiv:
        dbg.nospace() << "CircleSameDiv";
        break;
    case VolumeType::CircleFill:
        dbg.nospace() << "CircleFill";
        break;
    case VolumeType::Sphere:
        dbg.nospace() << "Sphere";
        break;
    case VolumeType::SphereSameDiv:
        dbg.nospace() << "SphereSameDiv";
        break;
    case VolumeType::SphereFill:
        dbg.nospace() << "SphereFill";
        break;
    case VolumeType::Cylinder:
        dbg.nospace() << "Cylinder";
        break;
    case VolumeType::CylinderFill:
        dbg.nospace() << "CylinderFill";
        break;
    case VolumeType::Box:
        dbg.nospace() << "Box";
        break;
    case VolumeType::BoxFill:
        dbg.nospace() << "BoxFill";
        break;
    case VolumeType::Line:
        dbg.nospace() << "Line";
        break;
    case VolumeType::LineSameDiv:
        dbg.nospace() << "LineSameDiv";
        break;
    case VolumeType::Rectangle:
        dbg.nospace() << "Rectangle";
        break;
    case VolumeType::VOLUME_TYPE_MAX:
        dbg.nospace() << "VOLUME_TYPE_MAX";
        break;
    default:
        dbg.nospace() << "Unknown";
        break;
    }

    return dbg;
}

QString toString(const VolumeType& type)
{
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
    case VolumeType::VOLUME_TYPE_MAX: return "VOLUME_TYPE_MAX";
    default: return "UNKNOWN";
    }
}


// ========================================================================== //

TextureFormat& assignFromInt(TextureFormat& e, int value) {

    e = static_cast<TextureFormat>(value);
    return e;
}

QDebug operator<<(QDebug dbg, const TextureFormat& type) {

    QDebugStateSaver stateSaver(dbg);

    switch (type) {
    case TextureFormat::RGBA8888:
        dbg.nospace() << "RGBA8888";
        break;
    case TextureFormat::RGB888:
        dbg.nospace() << "RGB888";
        break;
    case TextureFormat::RGBA5551:
        dbg.nospace() << "RGBA5551";
        break;
    case TextureFormat::RGB565:
        dbg.nospace() << "RGB565";
        break;
    case TextureFormat::RGBA4444:
        dbg.nospace() << "RGBA4444";
        break;
    case TextureFormat::LA88:
        dbg.nospace() << "LA88";
        break;
    case TextureFormat::HL8:
        dbg.nospace() << "HL8";
        break;
    case TextureFormat::L8:
        dbg.nospace() << "L8";
        break;
    case TextureFormat::A8:
        dbg.nospace() << "A8";
        break;
    case TextureFormat::LA44:
        dbg.nospace() << "LA44";
        break;
    case TextureFormat::L4:
        dbg.nospace() << "L4";
        break;
    case TextureFormat::A4:
        dbg.nospace() << "A4";
        break;
    case TextureFormat::ETC1:
        dbg.nospace() << "ETC1";
        break;
    case TextureFormat::ETC1_A4:
        dbg.nospace() << "ETC1_A4";
        break;
    default:
        dbg.nospace() << "Unknown";
        break;
    }

    return dbg;
}

QString toQString(TextureFormat type) {

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
    default: "Unknown";
    }
}


// ========================================================================== //


TextureWrap& assignFromInt(TextureWrap& e, int value) {

    e = static_cast<TextureWrap>(value);
    return e;
}

QDebug operator<<(QDebug dbg, const TextureWrap& type) {

    QDebugStateSaver stateSaver(dbg);

    switch (type) {
    case TextureWrap::ClampToEdge:
        dbg.nospace() << "ClampToEdge";
        break;
    case TextureWrap::ClampToBorder:
        dbg.nospace() << "ClampToBorder";
        break;
    case TextureWrap::Repeat:
        dbg.nospace() << "Repeat";
        break;
    case TextureWrap::MirroredRepeat:
        dbg.nospace() << "MirroredRepeat";
        break;
    default:
        dbg.nospace() << "Unknown";
        break;
    }

    return dbg;
}

const QString toString(const TextureWrap& type) {

    switch (type) {
    case TextureWrap::ClampToEdge:    return "ClampToEdge";
    case TextureWrap::ClampToBorder:  return "ClampToBorder";
    case TextureWrap::Repeat:         return "Repeat";
    case TextureWrap::MirroredRepeat: return "MirroredRepeat";
    default: return "Unknown";
    }
}


// ========================================================================== //


TextureMagFilter& assignFromInt(TextureMagFilter& e, int value) {

    e = static_cast<TextureMagFilter>(value);
    return e;
}

QDebug operator<<(QDebug dbg, const TextureMagFilter& type) {

    QDebugStateSaver stateSaver(dbg);

    switch (type) {
    case TextureMagFilter::Nearest:
        dbg.nospace() << "Nearest";
        break;
    case TextureMagFilter::Linear:
        dbg.nospace() << "Linear";
        break;
    default:
        dbg.nospace() << "Unknown";
        break;
    }

    return dbg;
}

const QString toString(const TextureMagFilter& type) {

    switch (type) {
    case TextureMagFilter::Nearest: return "Nearest";
    case TextureMagFilter::Linear:  return "Linear";
    default: return "Unknown";
    }
}


// ========================================================================== //


TextureMinFilter& assignFromInt(TextureMinFilter& e, int value) {
    e = static_cast<TextureMinFilter>(value);
    return e;
}

QDebug operator<<(QDebug dbg, const TextureMinFilter& type) {
    QDebugStateSaver stateSaver(dbg);

    switch (type) {
    case TextureMinFilter::Nearest:
        dbg.nospace() << "Nearest";
        break;
    case TextureMinFilter::NearestMipmapNearest:
        dbg.nospace() << "NearestMipmapNearest";
        break;
    case TextureMinFilter::NearestMipmapLinear:
        dbg.nospace() << "NearestMipmapLinear";
        break;
    case TextureMinFilter::Linear:
        dbg.nospace() << "Linear";
        break;
    case TextureMinFilter::LinearMipmapNearest:
        dbg.nospace() << "LinearMipmapNearest";
        break;
    case TextureMinFilter::LinearMipmapLinear:
        dbg.nospace() << "LinearMipmapLinear";
        break;
    default:
        dbg.nospace() << "Unknown";
        break;
    }

    return dbg;
}

const QString toString(const TextureMinFilter& type) {

    switch (type) {
    case TextureMinFilter::Nearest:              return "Nearest";
    case TextureMinFilter::NearestMipmapNearest: return "NearestMipmapNearest";
    case TextureMinFilter::NearestMipmapLinear:  return "NearestMipmapLinear";
    case TextureMinFilter::Linear:               return "Linear";
    case TextureMinFilter::LinearMipmapNearest:  return "LinearMipmapNearest";
    case TextureMinFilter::LinearMipmapLinear:   return "LinearMipmapLinear";
    default: return "Unknown";
    }
}


// ========================================================================== //


} // namespace Ptcl
