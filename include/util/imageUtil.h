#ifndef IMAGEUTIL_H
#define IMAGEUTIL_H

#include "typedefs.h"
#include "ptcl/ptclEnum.h"

#include "etc1/rg_etc1.h"

namespace Ptcl {
    class Texture;
}

#include <QColor>

namespace ImageUtil {
    QImage picaTextureToQImage(const std::vector<u8>& textureData, s32 width, s32 height, Ptcl::TextureFormat format);

    // Ptcl::Texture createTexture(const std::vector<u8>& textureData, s32 width, s32 height, Ptcl::TextureFormat format);

    // u32 getTextureSize(s32 width, s32 height, Ptcl::TextureFormat format, u32 alignment);

    // --------------------------------------------------------------------- //

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


} // namespace ImageUitl

#endif // IMAGEUTIL_H
