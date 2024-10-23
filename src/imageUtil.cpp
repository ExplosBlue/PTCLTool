#include "imageUtil.h"

#include "ptcl/ptclTexture.h"

#include "etc1/rg_etc1.h"

namespace ImageUtil {

namespace /* Anonymous */ {

constexpr u32 bitsPerPixel(Ptcl::TextureFormat format) {

    u32 bpp = 0;

    switch (format) {
    case Ptcl::TextureFormat::RGBA8888:
        bpp = 32;
        break;
    case Ptcl::TextureFormat::RGB888:
        bpp = 24;
        break;
    case Ptcl::TextureFormat::RGBA5551:
    case Ptcl::TextureFormat::RGB565:
    case Ptcl::TextureFormat::RGBA4444:
    case Ptcl::TextureFormat::LA88:
    case Ptcl::TextureFormat::HL8:
        bpp = 16;
        break;
    case Ptcl::TextureFormat::L8:
    case Ptcl::TextureFormat::A8:
    case Ptcl::TextureFormat::LA44:
    case Ptcl::TextureFormat::ETC1_A4:
        bpp = 8;
        break;
    case Ptcl::TextureFormat::L4:
    case Ptcl::TextureFormat::A4:
    case Ptcl::TextureFormat::ETC1:
        bpp = 4;
        break;
    default:
        break;
    }

    return bpp;
}

static const std::array swizzleLUT = {
     0,  1,  8,  9,  2,  3, 10, 11,
    16, 17, 24, 25, 18, 19, 26, 27,
     4,  5, 12, 13,  6,  7, 14, 15,
    20, 21, 28, 29, 22, 23, 30, 31,
    32, 33, 40, 41, 34, 35, 42, 43,
    48, 49, 56, 57, 50, 51, 58, 59,
    36, 37, 44, 45, 38, 39, 46, 47,
    52, 53, 60, 61, 54, 55, 62, 63
};

void flipVertical(u32 width, u32 height, u8* input) {

    u32 imgSize = width * height * 4;
    std::vector<u8> flippedOutput(imgSize);

    for (u32 y = 0; y < height; ++y) {
        for (u32 x = 0; x < width * 4; ++x) {
            flippedOutput[(height - 1 - y) * width * 4 + x] = input[y * width * 4 + x];
        }
    }
    std::copy(flippedOutput.begin(), flippedOutput.end(), input);
}

QImage getTextureRaster(const std::vector<u8>& textureData, s32 width, s32 height, Ptcl::TextureFormat format) {

    if (format == Ptcl::TextureFormat::ETC1 || format == Ptcl::TextureFormat::ETC1_A4) {

        std::vector<u8> decompressedData = ETC1Decompress(textureData, width, height, format == Ptcl::TextureFormat::ETC1_A4);
        QImage outputImg(decompressedData.data(), width, height, QImage::Format_RGBA8888);

        flipVertical(width, height, outputImg.bits());
        return outputImg.copy(); // ugly
    }

    QImage outputImg(width, height, QImage::Format_RGBA8888);
    u8* output = outputImg.bits();

    u32 increment = bitsPerPixel(format) / 8;

    if (increment == 0) {
        increment = 1;
    }

    u32 srcIdx = 0;

    for (u32 ty = 0; ty < height; ty += 8) {

        for (u32 tx = 0; tx < width; tx += 8) {

            for (u32 px : swizzleLUT) {

                u32 x = px & 7;
                u32 y = (px - x) >> 3;

                u32 outIdx = (tx + x + ((height - 1 - (ty + y)) * width)) * 4;

                switch (format) {
                case Ptcl::TextureFormat::RGBA8888:
                    output[outIdx + 0] = textureData[srcIdx + 3];
                    output[outIdx + 1] = textureData[srcIdx + 2];
                    output[outIdx + 2] = textureData[srcIdx + 1];
                    output[outIdx + 3] = textureData[srcIdx + 0];
                    break;
                case Ptcl::TextureFormat::RGB888:
                    output[outIdx + 0] = textureData[srcIdx + 2];
                    output[outIdx + 1] = textureData[srcIdx + 1];
                    output[outIdx + 2] = textureData[srcIdx + 0];
                    output[outIdx + 3] = 0xFF;
                    break;
                case Ptcl::TextureFormat::RGBA5551: {
                        u16 value = (textureData[srcIdx + 0] << 0 | textureData[srcIdx + 1] << 8);

                        u32 r = ((value >>  1) & 0x1f) << 3;
                        u32 g = ((value >>  6) & 0x1f) << 3;
                        u32 b = ((value >> 11) & 0x1f) << 3;
                        u32 a = value & 1;

                        output[outIdx + 0] = b | (b >> 5);
                        output[outIdx + 1] = g | (g >> 5);
                        output[outIdx + 2] = r | (r >> 5);
                        output[outIdx + 3] = a * 0xff;
                    }
                    break;
                case Ptcl::TextureFormat::RGB565: {
                        u16 value = (textureData[srcIdx + 0] << 0 | textureData[srcIdx + 1] << 8);

                        u32 r = ((value >>  0) & 0x1f) << 3;
                        u32 g = ((value >>  5) & 0x3f) << 2;
                        u32 b = ((value >> 11) & 0x1f) << 3;

                        output[outIdx + 0] = b | (b >> 5);
                        output[outIdx + 1] = g | (g >> 6);
                        output[outIdx + 2] = r | (r >> 5);
                        output[outIdx + 3] = 0xff;
                    }
                    break;
                case Ptcl::TextureFormat::RGBA4444: {
                        u16 value = (textureData[srcIdx + 0] << 0 | textureData[srcIdx + 1] << 8);

                        u32 r = ((value >>  4) & 0xF);
                        u32 g = ((value >>  8) & 0xF);
                        u32 b = ((value >> 12) & 0xF);
                        u32 a = (value & 0xF);

                        output[outIdx + 0] = b | (b << 4);
                        output[outIdx + 1] = g | (g << 4);
                        output[outIdx + 2] = r | (r << 4);
                        output[outIdx + 3] = a | (a << 4);
                    }
                    break;
                case Ptcl::TextureFormat::LA88:
                    output[outIdx + 0] = textureData[srcIdx + 1];
                    output[outIdx + 1] = textureData[srcIdx + 1];
                    output[outIdx + 2] = textureData[srcIdx + 1];
                    output[outIdx + 3] = textureData[srcIdx + 0];
                    break;
                case Ptcl::TextureFormat::HL8:
                    output[outIdx + 0] = textureData[srcIdx + 1];
                    output[outIdx + 1] = textureData[srcIdx + 0];
                    output[outIdx + 2] = 0x00;
                    output[outIdx + 3] = 0xFF;
                    break;
                case Ptcl::TextureFormat::L8:
                    output[outIdx + 0] = textureData[srcIdx];
                    output[outIdx + 1] = textureData[srcIdx];
                    output[outIdx + 2] = textureData[srcIdx];
                    output[outIdx + 3] = 0xFF;
                    break;
                case Ptcl::TextureFormat::A8:
                    output[outIdx + 0] = 0xFF;
                    output[outIdx + 1] = 0xFF;
                    output[outIdx + 2] = 0xFF;
                    output[outIdx + 3] = textureData[srcIdx];
                    break;
                case Ptcl::TextureFormat::LA44:
                    output[outIdx + 0] = static_cast<u8>((textureData[srcIdx] >> 4) | (textureData[srcIdx] & 0xF0));
                    output[outIdx + 1] = static_cast<u8>((textureData[srcIdx] >> 4) | (textureData[srcIdx] & 0xF0));
                    output[outIdx + 2] = static_cast<u8>((textureData[srcIdx] >> 4) | (textureData[srcIdx] & 0xF0));
                    output[outIdx + 3] = static_cast<u8>((textureData[srcIdx] << 4) | (textureData[srcIdx] & 0x0F));
                    break;
                case Ptcl::TextureFormat::L4: {
                    u32 luminance = (textureData[srcIdx >> 1] >> ((srcIdx & 1) << 2)) & 0xF;
                    output[outIdx + 0] = static_cast<u8>(luminance << 4 | luminance);
                    output[outIdx + 1] = static_cast<u8>(luminance << 4 | luminance);
                    output[outIdx + 2] = static_cast<u8>(luminance << 4 | luminance);
                    output[outIdx + 3] = 0xFF;
                    }
                    break;
                case Ptcl::TextureFormat::A4: {
                    u32 alpha = (textureData[srcIdx >> 1] >> ((srcIdx & 1) << 2)) & 0xF;
                    output[outIdx + 0] = 0xFF;
                    output[outIdx + 1] = 0xFF;
                    output[outIdx + 2] = 0xFF;
                    output[outIdx + 3] = static_cast<u8>(alpha << 4 | alpha);
                    }
                    break;
                default:
                    break;
                }

                srcIdx += increment;
            }
        }
    }

    flipVertical(width, height, output);
    return outputImg;
}

} // namespace Anonymous

QImage picaTextureToQImage(const std::vector<u8>& textureData, s32 width, s32 height, Ptcl::TextureFormat format) {

    QImage texture = QImage(width, height, QImage::Format_RGBA8888);

    texture = getTextureRaster(textureData, width, height, format);

    return texture;
}

} // namespace ImageUtil
