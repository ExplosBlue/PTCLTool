#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include "typedefs.h"
#include "ptcl/ptclEnum.h"

namespace Ptcl {
    class Texture;
}

namespace ImageUtil {
    QImage picaTextureToQImage(const QByteArray& textureData, s32 width, s32 height, Ptcl::TextureFormat format);

    Ptcl::Texture createTexture(const QByteArray& textureData, u32 width, u32 height, Ptcl::TextureFormat format);

    u32 getTextureSize(s32 width, s32 height, Ptcl::TextureFormat format, u32 alignment);

} // namespace ImageUitl

#endif // IMAGEUTIL_H
