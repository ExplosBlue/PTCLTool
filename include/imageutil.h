#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include "typedefs.h"

#include "gr.h"

class QImage;

namespace Ptcl {
    class Texture;
}

class QByteArray;

namespace ImageUtil {
    QImage picaTextureToQImage(const QByteArray& textureData, s32 width, s32 height, gr::PicaDataTextureFormat format);

    Ptcl::Texture createTexture(const QByteArray& textureData, u32 width, u32 height, gr::PicaDataTextureFormat format);

    u32 getTextureSize(s32 width, s32 height, gr::PicaDataTextureFormat format, u32 alignment);

} // namespace ImageUitl

#endif // IMAGEUTIL_H
