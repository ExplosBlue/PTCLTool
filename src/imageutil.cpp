#include "imageutil.h"

#include "ptcl/ptclTexture.h"

namespace ImageUtil {

namespace /* Anonymous */ {

quint32 align(quint32 value, quint32 alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

u32 clampColor(u32 val) {
    if (val > 255) return 255;
    if (val < 0) return 0;
    return val;
}

void getTextureRaster(const QByteArray& textureData, QImage* image, bool hasAlpha, Ptcl::TextureFormat format) {

    quint8* lineData = image->scanLine(0);
    bool premultiply = (image->format() == QImage::Format_RGBA8888_Premultiplied);

    QDataStream stream(textureData);
    stream.setByteOrder(QDataStream::LittleEndian);

    for (quint32 y = 0; y < image->height(); y += 8)
    {
        for (quint32 x = 0; x < image->width(); x += 8)
        {
            for (quint32 ty = 0; ty < 8; ty += 4)
            {
                for (quint32 tx = 0; tx < 8; tx += 4)
                {
                    for (quint32 sy = 0; sy < 4; sy += 2)
                    {
                        for (quint32 sx = 0; sx < 4; sx += 2)
                        {
                            for (quint32 yy = 0; yy < 2; yy++)
                            {
                                for (quint32 xx = 0; xx < 2; xx++)
                                {

                                    quint32 r, g, b, a;

                                    switch (format)
                                    {
                                    case Ptcl::TextureFormat::RGBA4444:
                                    {
                                        quint16 i;
                                        stream >> i;
                                        r = ((i >> 12) & 0xF) << 4;
                                        g = ((i >> 8) & 0xF) << 4;
                                        b = ((i >> 4) & 0xF) << 4;
                                        a = (i & 0xF) << 4;
                                        break;
                                    }
                                    case Ptcl::TextureFormat::RGBA5551:
                                    {
                                        quint16 i;
                                        stream >> i;
                                        r = ((i >> 11) & 0x1F) << 3;
                                        g = ((i >> 6) & 0x1F) << 3;
                                        b = ((i >> 1) & 0x1F) << 3;
                                        if (i & 1)
                                            a = 255;
                                        else
                                            a = 0;
                                        break;
                                    }
                                    case Ptcl::TextureFormat::RGBA8888:
                                    {
                                        u8 temp;
                                        stream >> temp;
                                        a = temp;

                                        stream >> temp;
                                        b = temp;

                                        stream >> temp;
                                        g = temp;

                                        stream >> temp;
                                        r = temp;
                                        break;
                                    }
                                    case Ptcl::TextureFormat::RGB565:
                                    {
                                        quint16 i;
                                        stream >> i;
                                        r = ((i >> 11) & 0x1F) << 3;
                                        g = ((i >> 5) & 0x3F) << 2;
                                        b = (i & 0x1F) << 3;
                                        break;
                                    }
                                    case Ptcl::TextureFormat::RGB888:
                                    {
                                        u8 temp;
                                        stream >> temp;
                                        b = temp;

                                        stream >> temp;
                                        g = temp;

                                        stream >> temp;
                                        r = temp;
                                        break;
                                    }
                                    case Ptcl::TextureFormat::L8: // TODO: Check this
                                    {
                                        quint8 luminance;
                                        stream >> luminance;
                                        r = g = b = luminance;
                                        a = 255;  // Opaque since there's no alpha in L8
                                        break;
                                    }
                                    // case Ptcl::TextureFormat::A8: // TODO: Check this
                                    // {
                                    //     quint8 alpha;
                                    //     stream >> alpha;
                                    //     r = g = b = 0;  // Set R, G, B to 0 (black)
                                    //     a = alpha;      // Set the alpha value
                                    //     break;
                                    // }
                                    case Ptcl::TextureFormat::LA88: // TODO: Check this
                                    {
                                        quint8 luminance, alpha;
                                        stream >> luminance >> alpha;
                                        r = g = b = luminance;  // Set R, G, B to the luminance value
                                        a = alpha;              // Set the alpha value
                                        break;
                                    }
                                        // case Ptcl::TextureFormat::A4: // TODO: Check this
                                        // {
                                        //     quint8 alphaPair;
                                        //     stream >> alphaPair;

                                        //     // First 4-bit alpha (upper nibble)
                                        //     quint8 alpha1 = (alphaPair >> 4) * 17;  // Scale from 0-15 to 0-255
                                        //     r = g = b = 0;  // Set R, G, B to black
                                        //     a = alpha1;

                                        //     // Write the first pixel
                                        //     quint32 dstpos1 = ((yy + sy + ty + y) * image->width() + xx + sx + tx + x) * 4;
                                        //     lineData[dstpos1 + 0] = r;
                                        //     lineData[dstpos1 + 1] = g;
                                        //     lineData[dstpos1 + 2] = b;
                                        //     lineData[dstpos1 + 3] = a;

                                        //     // Second 4-bit alpha (lower nibble)
                                        //     quint8 alpha2 = (alphaPair & 0xF) * 17;  // Scale from 0-15 to 0-255
                                        //     r = g = b = 0;  // Set R, G, B to black
                                        //     a = alpha2;

                                        //     // Write the second pixel (increment positions as needed)
                                        //     quint32 dstpos2 = ((yy + sy + ty + y) * image->width() + xx + sx + tx + x + 1) * 4;
                                        //     lineData[dstpos2 + 0] = r;
                                        //     lineData[dstpos2 + 1] = g;
                                        //     lineData[dstpos2 + 2] = b;
                                        //     lineData[dstpos2 + 3] = a;

                                        //     break;
                                        // }
                                    case Ptcl::TextureFormat::LA44: // TODO: Check this
                                    {
                                        quint8 la;
                                        stream >> la;

                                        // Extract luminance (upper 4 bits) and scale to 8-bit
                                        quint8 luminance = (la >> 4) * 17;
                                        r = g = b = luminance;  // Set R, G, B to luminance value

                                        // Extract alpha (lower 4 bits) and scale to 8-bit
                                        a = (la & 0xF) * 17;

                                        break;
                                    }
                                    default:
                                    {
                                        throw std::runtime_error("ImageUtil: Unsupported Texture Format");
                                        break;
                                    }
                                    }

                                    if (hasAlpha)
                                    {
                                        quint32 dstpos = ((yy + sy + ty + y) * image->width() + xx + sx + tx + x) * 4;

                                        if (premultiply)
                                        {
                                            r = (r * a) / 255;
                                            g = (g * a) / 255;
                                            b = (b * a) / 255;
                                        }

                                        lineData[dstpos + 0] = r;
                                        lineData[dstpos + 1] = g;
                                        lineData[dstpos + 2] = b;
                                        lineData[dstpos + 3] = a;
                                    }
                                    else
                                    {
                                        quint32 dstpos = ((yy + sy + ty + y) * image->width() + xx + sx + tx + x) * 3;
                                        lineData[dstpos + 0] = r;
                                        lineData[dstpos + 1] = g;
                                        lineData[dstpos + 2] = b;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}


void getTextureETC1(const QByteArray& textureData, QImage* image, bool hasAlpha) {

    const std::array<std::array<s32, 2>, 8> etc1_mod =
        {{
            {2, 8}, {5, 17}, {9, 29}, {13, 42},
            {18, 60}, {24, 80}, {33, 106}, {47, 183}
        }};

    u8* lineData = image->scanLine(0);
    bool premultiply = (image->format() == QImage::Format_RGBA8888_Premultiplied);

    QDataStream stream(textureData);
    stream.setByteOrder(QDataStream::LittleEndian);

    for (quint32 y = 0; y < image->height(); y += 8)
    {
        for (quint32 x = 0; x < image->width(); x += 8)
        {
            for (quint32 ty = 0; ty < 8; ty += 4)
            {
                for (quint32 tx = 0; tx < 8; tx += 4)
                {
                    quint64 alpha = 0;
                    if (hasAlpha)
                    {
                        u32 temp;
                        stream >> temp;

                        alpha = temp;
                        stream >> temp;
                        alpha |= ((quint64)temp << 32);
                    }

                    quint16 subindexes;
                    stream >> subindexes;
                    quint16 negative;
                    stream >> negative;
                    quint32 flags_col;
                    stream >> flags_col;

                    for (quint32 sx = 0; sx < 4; sx++)
                    {
                        for (quint32 sy = 0; sy < 4; sy++)
                        {
                            qint32 r, g, b;
                            quint32 tsx;

                            // dirty
                            if (flags_col & 0x1)
                                tsx = sy;
                            else
                                tsx = sx;

                            if (flags_col & 0x2)
                            {
                                r = (flags_col & 0xF8000000) >> 24;
                                g = (flags_col & 0x00F80000) >> 16;
                                b = (flags_col & 0x0000F800) >> 8;
                                if (tsx >= 2)
                                {
                                    qint32 dr = (flags_col & 0x07000000) >> 21;
                                    qint32 dg = (flags_col & 0x00070000) >> 13;
                                    qint32 db = (flags_col & 0x00000700) >> 5;
                                    if (dr & 0x20) dr -= 0x40;
                                    if (dg & 0x20) dg -= 0x40;
                                    if (db & 0x20) db -= 0x40;
                                    r += dr;
                                    g += dg;
                                    b += db;
                                }

                                r |= (r >> 5);
                                g |= (g >> 5);
                                b |= (b >> 5);
                            }
                            else
                            {
                                if (tsx >= 2)
                                {
                                    r = (flags_col & 0x0F000000) >> 20;
                                    g = (flags_col & 0x000F0000) >> 12;
                                    b = (flags_col & 0x00000F00) >> 4;
                                }
                                else
                                {
                                    r = (flags_col & 0xF0000000) >> 24;
                                    g = (flags_col & 0x00F00000) >> 16;
                                    b = (flags_col & 0x0000F000) >> 8;
                                }

                                r |= (r >> 4);
                                g |= (g >> 4);
                                b |= (b >> 4);
                            }

                            quint32 mod_index = (flags_col >> (tsx>=2 ? 2:5)) & 0x7;
                            qint32 mod = etc1_mod[mod_index][subindexes & 0x1];
                            if (negative & 0x1) mod = -mod;

                            r = clampColor(r + mod);
                            g = clampColor(g + mod);
                            b = clampColor(b + mod);



                            if (hasAlpha)
                            {
                                quint8 a = alpha & 0xF;
                                a |= (a << 4);

                                quint32 dstpos = ((sy + ty + y) * image->width() + sx + tx + x) * 4;

                                if (premultiply)
                                {
                                    r = (r * a) / 255;
                                    g = (g * a) / 255;
                                    b = (b * a) / 255;
                                }

                                lineData[dstpos + 0] = r;
                                lineData[dstpos + 1] = g;
                                lineData[dstpos + 2] = b;
                                lineData[dstpos + 3] = a;
                            }
                            else
                            {
                                quint32 dstpos = ((sy + ty + y) * image->height() + sx + tx + x) * 3;
                                lineData[dstpos + 0] = r;
                                lineData[dstpos + 1] = g;
                                lineData[dstpos + 2] = b;
                            }

                            subindexes >>= 1;
                            negative >>= 1;
                            alpha >>= 4ULL;
                        }
                    }
                }
            }
        }
    }

}

} // namespace Anonymous

QImage picaTextureToQImage(const QByteArray& textureData, s32 width, s32 height, Ptcl::TextureFormat format) {

    bool hasAlpha = false;

    switch (format) {
    case Ptcl::TextureFormat::RGBA4444:
    case Ptcl::TextureFormat::RGBA5551:
    case Ptcl::TextureFormat::ETC1_A4:
        hasAlpha = true;
        break;
    default:
        break;
    }

    QImage::Format imageFormat = (hasAlpha) ? QImage::Format_RGBA8888 : QImage::Format_RGB888;
    QImage texture = QImage(width, height, imageFormat);

    try {

        switch (format) {
        case Ptcl::TextureFormat::ETC1:
        case Ptcl::TextureFormat::ETC1_A4:
            getTextureETC1(textureData, &texture, hasAlpha);
            break;
        default:
            getTextureRaster(textureData, &texture, hasAlpha, format);
            break;
        }

    } catch(std::exception& e) {
        qDebug() << e.what() << format;
    }

    return texture;
}

Ptcl::Texture createTexture(const QByteArray& textureData, u32 width, u32 height, Ptcl::TextureFormat format) {

    QImage image = picaTextureToQImage(textureData, width, height, format);
    return {std::move(image), format};
}

u32 getTextureSize(s32 width, s32 height, Ptcl::TextureFormat format, u32 alignment)
{
    u32 bitsPerPixel = 0;

    switch (format)
    {
    case Ptcl::TextureFormat::RGBA4444:
    case Ptcl::TextureFormat::RGBA5551:
    case Ptcl::TextureFormat::RGB565:
    case Ptcl::TextureFormat::LA88:
        bitsPerPixel = 16;
        break;

    case Ptcl::TextureFormat::RGBA8888:
        bitsPerPixel = 32;
        break;

    case Ptcl::TextureFormat::RGB888:
        bitsPerPixel = 24;
        break;

    case Ptcl::TextureFormat::L8:
    case Ptcl::TextureFormat::A8:
    case Ptcl::TextureFormat::LA44:
        bitsPerPixel = 8;
        break;

    case Ptcl::TextureFormat::A4:
        bitsPerPixel = 4;
        break;

    case Ptcl::TextureFormat::ETC1:
    {
        int widthBlocks = (width + 3) / 4;
        int heightBlocks = (height + 3) / 4;
        return widthBlocks * heightBlocks * 8;
    }

    case Ptcl::TextureFormat::ETC1_A4:
    {
        int widthBlocks = (width + 3) / 4;
        int heightBlocks = (height + 3) / 4;
        return widthBlocks * heightBlocks * 16;
    }

    default:
        // throw std::runtime_error("Unsupported Texture Format");
        return 0;
    }

    u32 rowSize = (width * bitsPerPixel) / 8;
    u32 alignedRowSize = align(rowSize, alignment);
    return alignedRowSize * height;
}

} // namespace ImageUtil
