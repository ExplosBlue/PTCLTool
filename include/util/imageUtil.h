#pragma once

#include "typedefs.h"
#include "ptcl/ptclEnum.h"

#include <QColor>
#include <QImage>


namespace ImageUtil {


// ========================================================================== //


enum class ETC1Quality {
    LowQuality,
    MediumQuality,
    HighQuality
};


// ========================================================================== //


QImage picaTextureToQImage(const std::vector<u8>& textureData, s32 width, s32 height, Ptcl::TextureFormat format);
std::vector<u8> QImageToPicaTexture(const QImage& image, Ptcl::TextureFormat format, ETC1Quality etcQuality = ETC1Quality::HighQuality, bool etcDither = false);


// ========================================================================== //


} // namespace ImageUitl
