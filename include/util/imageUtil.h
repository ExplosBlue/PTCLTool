#pragma once

#include "typedefs.h"
#include "ptcl/ptclEnum.h"

#include <QColor>
#include <QImage>

#include <atomic>


namespace ImageUtil {


// ========================================================================== //


enum class ETC1Quality {
    LowQuality,
    MediumQuality,
    HighQuality
};


// ========================================================================== //


inline const std::atomic<bool>& neverCancel() {
    static std::atomic<bool> flag{false};
    return flag;
}


// ========================================================================== //


QImage picaTextureToQImage(const std::vector<u8>& textureData, s32 width, s32 height, Ptcl::TextureFormat format);
std::vector<u8> QImageToPicaTexture(const QImage& image, Ptcl::TextureFormat format, ETC1Quality etcQuality = ETC1Quality::HighQuality, bool etcDither = false, const std::atomic<bool>& cancelFlag = neverCancel());

bool isValidTextureSize(s32 width, s32 height);
s32 nearestValidTextureSize(s32 value);

constexpr u32 maxTextureDimension() {
    return 4096;
}

constexpr u64 maxTextureBytes() {
    return 64ull << 20;
}

u64 textureDataMinBytes(s32 width, s32 height, Ptcl::TextureFormat format);


// ========================================================================== //


} // namespace ImageUtil
