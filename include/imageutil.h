#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include <QImage>

#include "typedefs.h"
#include "gr.h"

class ImageUtil
{
public:
    static QImage picaTextureToQImage(const QByteArray& textureData, u32 width, u32 height, gr::PicaDataTextureFormat format);

    static u32 getTextureSize(s32 width, s32 height, gr::PicaDataTextureFormat format, u32 alignment);

private:
    static void getTextureRaster(const QByteArray& textureData, QImage* image, bool hasAlpha, gr::PicaDataTextureFormat format);
    static void getTextureETC1(const QByteArray& textureData, QImage* image, bool hasAlpha);

    static inline quint32 align(quint32 value, quint32 alignment)
    {
        return (value + alignment - 1) & ~(alignment - 1);
    }

    static inline u32 clampColor(u32 val)
    {
        if (val > 255) return 255;
        if (val < 0) return 0;
        return val;
    }
};

#endif // IMAGEUTIL_H
