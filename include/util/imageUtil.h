#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include "typedefs.h"
#include "ptcl/ptclEnum.h"

namespace Ptcl {
    class Texture;
}

#include <QColor>

namespace ImageUtil {
    QImage picaTextureToQImage(const std::vector<u8>& textureData, s32 width, s32 height, Ptcl::TextureFormat format);

    // Ptcl::Texture createTexture(const std::vector<u8>& textureData, s32 width, s32 height, Ptcl::TextureFormat format);

    // u32 getTextureSize(s32 width, s32 height, Ptcl::TextureFormat format, u32 alignment);

    // --------------------------------------------------------------------- //

    inline std::array<std::array<int, 4>, 8> ETC1LUT = {{
        {{  2,    8,   -2,   -8 }},
        {{  5,   17,   -5,  -17 }},
        {{  9,   29,   -9,  -29 }},
        {{ 13,   42,  -13,  -42 }},
        {{ 18,   60,  -18,  -60 }},
        {{ 24,   80,  -24,  -80 }},
        {{ 33,  106,  -33, -106 }},
        {{ 47,  183,  -47, -183 }}
    }};

    inline u8 saturate(s32 value) {

        if (value > std::numeric_limits<u8>::max()) return std::numeric_limits<u8>::max();
        if (value < std::numeric_limits<u8>::min()) return std::numeric_limits<u8>::min();

        return static_cast<u8>(value);
    }

    inline QColor ETC1Pixel(u32 r, u32 g, u32 b, u32 x, u32 y, u32 block, u32 table) {

        s32 index = x * 4 + y;
        u32 msb = block << 1;

        s32 pixel = index < 8 ?
                        ETC1LUT[table][((block >> (index + 24)) & 1) + ((msb >> (index + 8)) & 2)] :
                        ETC1LUT[table][((block >> (index + 8)) & 1) + ((msb >> (index - 8)) & 2)];

        r = saturate(static_cast<s32>(r + pixel));
        g = saturate(static_cast<s32>(g + pixel));
        b = saturate(static_cast<s32>(b + pixel));

        return QColor::fromRgb(r, g, b);
    }

    inline std::vector<u8> ETC1Tile(u64 block) {

        u32 blockLow = static_cast<u32>(block >> 32);
        u32 blockHigh = static_cast<u32>(block >> 0);

        bool flip = (blockHigh & 0x1000000) != 0;
        bool diff = (blockHigh & 0x2000000) != 0;

        u32 r1, g1, b1;
        u32 r2, g2, b2;

        if (diff) {

            b1 = (blockHigh & 0x0000f8) >>  0;
            g1 = (blockHigh & 0x00f800) >>  8;
            r1 = (blockHigh & 0xf80000) >> 16;

            b2 = static_cast<u32>(static_cast<s8>(b1 >> 3) + (static_cast<s8>((blockHigh & 0x000007) <<  5) >> 5));
            g2 = static_cast<u32>(static_cast<s8>(g1 >> 3) + (static_cast<s8>((blockHigh & 0x000700) >>  3) >> 5));
            r2 = static_cast<u32>(static_cast<s8>(r1 >> 3) + (static_cast<s8>((blockHigh & 0x070000) >> 11) >> 5));

            b1 |= b1 >> 5;
            g1 |= g1 >> 5;
            r1 |= r1 >> 5;

            b2 = (b2 << 3) | (b2 >> 2);
            g2 = (g2 << 3) | (g2 >> 2);
            r2 = (r2 << 3) | (r2 >> 2);
        }
        else {

            b1 = (blockHigh & 0x0000f0) >>  0;
            g1 = (blockHigh & 0x00f000) >>  8;
            r1 = (blockHigh & 0xf00000) >> 16;

            b2 = (blockHigh & 0x00000f) >>  4;
            g2 = (blockHigh & 0x000f00) >>  4;
            r2 = (blockHigh & 0x0f0000) >> 12;

            b1 |= b1 >> 4;
            g1 |= g1 >> 4;
            r1 |= r1 >> 4;

            b2 |= b2 >> 4;
            g2 |= g2 >> 4;
            r2 |= r2 >> 4;
        }

        u32 table1 = (blockHigh >> 29) & 7;
        u32 table2 = (blockHigh >> 26) & 7;

        std::vector<u8> output(64, 0);

        if (!flip) {

            for (s32 y = 0; y < 4; y++) {

                for (s32 x = 0; x < 2; x++) {

                    QColor color1 = ETC1Pixel(r1, g1, b1, x + 0, y, blockLow, table1);
                    QColor color2 = ETC1Pixel(r2, g2, b2, x + 2, y, blockLow, table2);

                    s32 offset1 = (y * 4 + x) * 4;

                    output[offset1 + 0] = color1.blue();
                    output[offset1 + 1] = color1.green();
                    output[offset1 + 2] = color1.red();

                    s32 offset2 = (y * 4 + x + 2) * 4;

                    output[offset2 + 0] = color2.blue();
                    output[offset2 + 1] = color2.green();
                    output[offset2 + 2] = color2.red();
                }
            }
        }
        else {

            for (s32 y = 0; y < 2; y++) {

                for (s32 x = 0; x < 4; x++) {

                    QColor color1 = ETC1Pixel(r1, g1, b1, x, y + 0, blockLow, table1);
                    QColor color2 = ETC1Pixel(r2, g2, b2, x, y + 2, blockLow, table2);

                    s32 offset1 = (y * 4 + x) * 4;

                    output[offset1 + 0] = color1.blue();
                    output[offset1 + 1] = color1.green();
                    output[offset1 + 2] = color1.red();

                    s32 offset2 = ((y + 2) * 4 + x) * 4;

                    output[offset2 + 0] = color2.blue();
                    output[offset2 + 1] = color2.green();
                    output[offset2 + 2] = color2.red();
                }
            }
        }

        return output;
    }


    inline constexpr std::array XT = { 0, 4, 0, 4 };
    inline constexpr std::array YT = { 0, 0, 4, 4 };

    inline u64 Swap64(u64 value)
    {
        value = ((value & 0xFFFFFFFF00000000) >> 32) | ((value & 0x00000000FFFFFFFF) << 32);
        value = ((value & 0xFFFF0000FFFF0000) >> 16) | ((value & 0x0000FFFF0000FFFF) << 16);
        value = ((value & 0xFF00FF00FF00FF00) >>  8) | ((value & 0x00FF00FF00FF00FF) <<  8);

        return value;
    }

    inline void blockCopy(const void* src, std::size_t srcOffset, void* dst, std::size_t dstOffset, std::size_t count) {
        const uint8_t* srcPtr = static_cast<const uint8_t*>(src) + srcOffset;
        uint8_t* dstPtr = static_cast<uint8_t*>(dst) + dstOffset;

        std::memcpy(dstPtr, srcPtr, count);
    }

    inline std::vector<u8> ETC1Decompress(const std::vector<u8>& input, s32 width, s32 height, bool alpha) {

        std::vector<u8> output(width * height * 4, 0);

        s32 srcOffs = 0;

        for (s32 ty = 0; ty < height; ty += 8) {

            for (s32 tx = 0; tx < width; tx += 8) {

                for (s32 t = 0; t < 4; t++) {

                    u64 alphaBlock = 0xFFFFFFFFFFFFFFFF;

                    if (alpha) {

                        std::memcpy(&alphaBlock, &input[srcOffs], sizeof(u64));
                        srcOffs += sizeof(u64);
                    }

                    u64 colorBlock = 0xFFFFFFFFFFFFFFFF;
                    std::memcpy(&colorBlock, &input[srcOffs], sizeof(u64));
                    srcOffs += sizeof(u64);
                    colorBlock = Swap64(colorBlock);

                    std::vector<u8> tile = ETC1Tile(colorBlock);

                    s32 tileOffset = 0;

                    for (s32 py = YT[t]; py < 4 + YT[t]; py++) {

                        for (s32 px = XT[t]; px < 4 + XT[t]; px++) {

                            s32 outOffs = ((height - 1 - (ty + py)) * width + tx + px) * 4;

                            // blockCopy(tile.data(), tileOffset, output.data(), outOffs, 3);
                            std::copy(tile.begin() + tileOffset, tile.begin() + tileOffset + 3, output.begin() + outOffs);

                            s32 alphaShift = ((px & 3) * 4 + (py & 3)) << 2;

                            u8 a = static_cast<u8>((alphaBlock >> alphaShift) & 0xF);

                            output[outOffs + 3] = static_cast<u8>((a << 4) | a);

                            tileOffset += 4;
                        }
                    }
                }
            }
        }

        return output;
    }


} // namespace ImageUitl

#endif // IMAGEUTIL_H
