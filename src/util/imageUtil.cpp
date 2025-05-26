#include "util/imageUtil.h"

#include <QImage>

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

inline constexpr std::array XT = { 0, 4, 0, 4 };
inline constexpr std::array YT = { 0, 0, 4, 4 };

inline u64 Swap64(u64 value) {
    return ((value & 0x00000000000000FFULL) << 56) |
           ((value & 0x000000000000FF00ULL) << 40) |
           ((value & 0x0000000000FF0000ULL) << 24) |
           ((value & 0x00000000FF000000ULL) << 8)  |
           ((value & 0x000000FF00000000ULL) >> 8)  |
           ((value & 0x0000FF0000000000ULL) >> 24) |
           ((value & 0x00FF000000000000ULL) >> 40) |
           ((value & 0xFF00000000000000ULL) >> 56);
}

inline std::vector<u8> ETC1Decompress(const std::vector<u8>& input, s32 width, s32 height, bool alpha) {
    std::vector<u8> output(static_cast<size_t>(width) * static_cast<size_t>(height) * 4, 0);
    s32 srcOffs = 0;

    for (s32 ty = 0; ty < height; ty += 8) {
        for (s32 tx = 0; tx < width; tx += 8) {
            for (s32 t = 0; t < 4; t++) {
                u64 alphaBlock = 0xFFFFFFFFFFFFFFFF;
                if (alpha) {
                    std::memcpy(&alphaBlock, &input[srcOffs], sizeof(u64));
                    srcOffs += sizeof(u64);
                }

                u64 colorBlock;
                std::memcpy(&colorBlock, &input[srcOffs], sizeof(u64));
                srcOffs += sizeof(u64);
                colorBlock = Swap64(colorBlock);
                std::array<u8, 8> etc1_block;
                std::memcpy(etc1_block.data(), &colorBlock, sizeof(etc1_block));

                std::array<u32, 16> decoded_rgba;
                rg_etc1::unpack_etc1_block(etc1_block.data(), decoded_rgba.data(), true);

                for (int i = 0; i < 16; ++i) {
                    int px = XT[t] + (i % 4);
                    int py = YT[t] + (i / 4);
                    int dstX = tx + px;
                    int dstY = ty + py;

                    if (dstX >= width || dstY >= height)
                        continue;

                    int outOffs = ((height - 1 - dstY) * width + dstX) * 4;

                    u32 pixel = decoded_rgba[i];
                    u8 r = (pixel >> 0) & 0xFF;
                    u8 g = (pixel >> 8) & 0xFF;
                    u8 b = (pixel >> 16) & 0xFF;

                    output[outOffs + 0] = r;
                    output[outOffs + 1] = g;
                    output[outOffs + 2] = b;

                    if (alpha) {
                        int alphaShift = ((px & 3) * 4 + (py & 3)) << 2;
                        u8 a = static_cast<u8>((alphaBlock >> alphaShift) & 0xF);
                        output[outOffs + 3] = static_cast<u8>((a << 4) | a);
                    } else {
                        output[outOffs + 3] = 255;
                    }
                }
            }
        }
    }

    return output;
}

inline std::vector<u8> ETC1Compress(const std::vector<u8>& rgba, s32 width, s32 height, bool alpha, ETC1Quality quality, bool dither) {
    std::vector<u8> output;

    for (s32 ty = 0; ty < height; ty += 8) {
        for (s32 tx = 0; tx < width; tx += 8) {
            for (int t = 0; t < 4; ++t) {
                std::array<u32, 16> block_rgba{};
                u64 alphaBlock = 0;

                for (int i = 0; i < 16; ++i) {
                    int px = XT[t] + (i % 4);
                    int py = YT[t] + (i / 4);
                    int dstX = tx + px;
                    int dstY = ty + py;

                    u8 r = 0, g = 0, b = 0, a = 255;

                    if (dstX < width && dstY < height) {
                        int srcIdx = (dstY * width + dstX) * 4;
                        r = rgba[srcIdx + 0];
                        g = rgba[srcIdx + 1];
                        b = rgba[srcIdx + 2];
                        a = rgba[srcIdx + 3];
                    }

                    // Force alpha to 255 because ETC1 does not support alpha
                    block_rgba[i] = (255 << 24) | (b << 16) | (g << 8) | r;

                    if (alpha) {
                        int alphaShift = ((px & 3) * 4 + (py & 3)) << 2;
                        u8 a4 = static_cast<u8>(a >> 4);
                        alphaBlock |= (static_cast<u64>(a4) & 0xF) << alphaShift;
                    }
                }

                // Compress color block
                std::array<u8, 8> compressed_color;
                rg_etc1::etc1_pack_params params;

                switch(quality) {
                case ImageUtil::ETC1Quality::LowQuality:
                    params.m_quality = rg_etc1::cLowQuality;
                case ImageUtil::ETC1Quality::MediumQuality:
                    params.m_quality = rg_etc1::cMediumQuality;
                default:
                    params.m_quality = rg_etc1::cHighQuality;
                }

                params.m_dithering = dither;

                rg_etc1::pack_etc1_block(compressed_color.data(), block_rgba.data(), params);

                // Write alpha block if needed
                if (alpha) {
                    std::array<u8, sizeof(u64)> aBytes{};
                    std::memcpy(aBytes.data(), &alphaBlock, sizeof(u64));
                    output.insert(output.end(), aBytes.begin(), aBytes.end());
                }

                // Write color block
                u64 cBlock;
                std::memcpy(&cBlock, compressed_color.data(), sizeof(u64));
                cBlock = Swap64(cBlock);
                std::array<u8, sizeof(u64)> cBytes{};
                std::memcpy(cBytes.data(), &cBlock, sizeof(u64));
                output.insert(output.end(), cBytes.begin(), cBytes.end());
            }
        }
    }

    return output;
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

std::vector<u8> QImageToPicaTexture(const QImage& image, Ptcl::TextureFormat format, ETC1Quality etcQuality, bool etcDither) {
    QImage rgbaImage = image.convertToFormat(QImage::Format_RGBA8888);
    s32 width = rgbaImage.width();
    s32 height = rgbaImage.height();

    if (format == Ptcl::TextureFormat::ETC1 || format == Ptcl::TextureFormat::ETC1_A4) {
        const u8* data = rgbaImage.constBits();
        size_t length = static_cast<size_t>(width) * static_cast<size_t>(height) * 4;
        std::vector<u8> pixelData(data, data + length);
        bool alpha = (format == Ptcl::TextureFormat::ETC1_A4);
        return ETC1Compress(pixelData, width, height, alpha, etcQuality, etcDither);
    }

    u32 increment = bitsPerPixel(format) / 8;
    if (increment == 0) {
        increment = 1;
    }

    std::vector<u8> textureData(static_cast<size_t>(width) * height * increment);
    u32 dstIdx = 0;

    static int nibbleToggle = 0;
    static u8 byteBuffer = 0;
    nibbleToggle = 0;
    byteBuffer = 0;

    for (u32 ty = 0; ty < height; ty += 8) {
        for (u32 tx = 0; tx < width; tx += 8) {
            for (u32 px : swizzleLUT) {

                u32 x = px & 7;
                u32 y = (px >> 3) & 7;

                u32 imgX = tx + x;
                u32 imgY = ty + y;

                // Bounds check
                if (imgX >= (u32)width || imgY >= (u32)height)
                    continue;

                QRgb pixel = rgbaImage.pixel(static_cast<int>(imgX), static_cast<int>(imgY));

                u8 r = qRed(pixel);
                u8 g = qGreen(pixel);
                u8 b = qBlue(pixel);
                u8 a = qAlpha(pixel);

                switch (format) {
                case Ptcl::TextureFormat::RGBA8888:
                    textureData[dstIdx + 0] = a;
                    textureData[dstIdx + 1] = b;
                    textureData[dstIdx + 2] = g;
                    textureData[dstIdx + 3] = r;
                    break;
                case Ptcl::TextureFormat::RGB888:
                    textureData[dstIdx + 0] = b;
                    textureData[dstIdx + 1] = g;
                    textureData[dstIdx + 2] = r;
                    break;
                case Ptcl::TextureFormat::RGBA5551: {
                    u16 R = (r >> 3);
                    u16 G = (g >> 3);
                    u16 B = (b >> 3);
                    u16 A = (a > 127 ? 1 : 0);
                    u16 val = (R << 11) | (G << 6) | (B << 1) | A;
                    textureData[dstIdx + 0] = val & 0xFF;
                    textureData[dstIdx + 1] = val >> 8;
                    break;
                }
                case Ptcl::TextureFormat::RGB565: {
                    u16 R = (r >> 3);
                    u16 G = (g >> 2);
                    u16 B = (b >> 3);
                    u16 val = (R << 11) | (G << 5) | B;
                    textureData[dstIdx + 0] = val & 0xFF;
                    textureData[dstIdx + 1] = val >> 8;
                    break;
                }
                case Ptcl::TextureFormat::RGBA4444: {
                    u16 R = r >> 4;
                    u16 G = g >> 4;
                    u16 B = b >> 4;
                    u16 A = a >> 4;
                    u16 val = (R << 12) | (G << 8) | (B << 4) | A;
                    textureData[dstIdx + 0] = val & 0xFF;
                    textureData[dstIdx + 1] = val >> 8;
                    break;
                }
                case Ptcl::TextureFormat::LA88: {
                    textureData[dstIdx + 0] = a;
                    textureData[dstIdx + 1] = static_cast<u8>((r + g + b) / 3);
                    break;
                }
                case Ptcl::TextureFormat::HL8: {
                    u8 l = r; // low byte stored in red
                    u8 h = g; // high byte stored in green

                    textureData[dstIdx + 0] = h;
                    textureData[dstIdx + 1] = l;
                    break;
                }
                case Ptcl::TextureFormat::L8: {
                    textureData[dstIdx + 0] = static_cast<u8>((r + g + b) / 3);
                    break;
                }
                case Ptcl::TextureFormat::A8: {
                    textureData[dstIdx + 0] = a;
                    break;
                }
                case Ptcl::TextureFormat::LA44: {
                    u8 luminance = static_cast<u8>(qGray(pixel) >> 4);  // 0–15
                    u8 alpha = static_cast<u8>(qAlpha(pixel) >> 4);     // 0–15
                    textureData[dstIdx] = static_cast<u8>((luminance << 4) | alpha);
                    break;
                }
                case Ptcl::TextureFormat::L4: {
                    u8 luminance = static_cast<u8>(qGray(pixel) >> 4); // Get 4-bit value
                    int byteIndex = dstIdx >> 1;
                    int shift = (dstIdx & 1) << 2; // 0 for even, 4 for odd (low nibble)

                    textureData[byteIndex] &= ~(0xF << shift);            // Clear target nibble
                    textureData[byteIndex] |= (luminance & 0xF) << shift; // Set nibble
                    break;
                }
                case Ptcl::TextureFormat::A4: {
                    u8 alpha = static_cast<u8>(qAlpha(pixel) >> 4);
                    int byteIndex = dstIdx >> 1;
                    int shift = (dstIdx & 1) << 2;

                    textureData[byteIndex] &= ~(0xF << shift);            // Clear target nibble
                    textureData[byteIndex] |= (alpha & 0xF) << shift;     // Set nibble
                    break;
                }
                default:
                    break;
                }

                dstIdx += increment;
            }
        }
    }

    return textureData;
}

} // namespace ImageUtil
