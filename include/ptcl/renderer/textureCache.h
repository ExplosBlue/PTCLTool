#pragma once

#include <unordered_map>

#include "typedefs.h"

class QImage;
class QOpenGLFunctions_2_0;

namespace Ptcl {
class Texture;
} // namespace Ptcl

namespace Ptcl::Render {

struct EmitterState;


// ========================================================================== //


struct TextureBinding {
    bool hasTexture{false};
    f32 texelWidth{0.0f};
    f32 texelHeight{0.0f};
};


// ========================================================================== //


class TextureCache {
public:
    TextureBinding acquire(QOpenGLFunctions_2_0& gl, const EmitterState& state);

    void clear(QOpenGLFunctions_2_0& gl);

private:
    struct CachedTexture {
        u32 id{0};
        u64 imageKey{0};
    };

    u32 upload(QOpenGLFunctions_2_0& gl, const QImage& source);

    void applySampler(QOpenGLFunctions_2_0& gl, const EmitterState& state, s32 maxMipLevel) const;

    std::unordered_map<const Ptcl::Texture*, CachedTexture> mTextures;
};


// ========================================================================== //


} // namespace Ptcl::Render
