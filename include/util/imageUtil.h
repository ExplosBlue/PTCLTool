#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include "typedefs.h"
#include "ptcl/ptclEnum.h"

namespace Ptcl {
    class Texture;
}

#include <QColor>

namespace ImageUtil {

enum class ETC1Quality {
    LowQuality,
    MediumQuality,
    HighQuality,

    MAX
};

template<typename EnumType>
const QString toString(const EnumType&) {
    static_assert(sizeof(EnumType) == 0, "toString not implemented for this enum type");
    return {};
}

template<>
inline const QString toString<ETC1Quality>(const ETC1Quality& type) {
    switch (type) {
    case ETC1Quality::LowQuality:    return "Low Quality";
    case ETC1Quality::MediumQuality: return "Medium Quality";
    case ETC1Quality::HighQuality:   return "High Quality";
    case ETC1Quality::MAX:           return "ETC1QUALITY_MAX";
    default: return "UNKNOWN";
    }
}

QImage picaTextureToQImage(const std::vector<u8>& textureData, s32 width, s32 height, Ptcl::TextureFormat format);
std::vector<u8> QImageToPicaTexture(const QImage& image, Ptcl::TextureFormat format, ETC1Quality etcQuality = ETC1Quality::HighQuality, bool etcDither = false);

} // namespace ImageUitl

#endif // IMAGEUTIL_H
