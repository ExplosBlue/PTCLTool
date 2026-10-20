#include "ptcl/renderer/textureCache.h"

#include <algorithm>
#include <bit>

#include <QImage>
#include <QOpenGLFunctions_2_0>

#include "ptcl/ptclTexture.h"
#include "ptcl/renderer/drawFrame.h"
#include "ptcl/renderer/renderState.h"


namespace Ptcl::Render {


// ========================================================================== //


u32 TextureCache::upload(QOpenGLFunctions_2_0& gl, const QImage& source) {
    const QImage image = source.convertToFormat(QImage::Format_RGBA8888);

    if (image.isNull()) {
        return 0;
    }

    u32 id = 0;
    gl.glGenTextures(1, &id);
    if (id == 0) {
        return 0;
    }

    gl.glBindTexture(GL_TEXTURE_2D, id);

    QImage level = image;
    for (s32 levelIndex = 0;; ++levelIndex) {
        gl.glTexImage2D(
            GL_TEXTURE_2D,
            levelIndex,
            GL_RGBA8,
            level.width(),
            level.height(),
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            level.constBits()
        );

        if (level.width() == 1 && level.height() == 1) {
            break;
        }

        level = level.scaled(
            std::max(1, level.width() / 2),
            std::max(1, level.height() / 2),
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation
        );
    }

    return id;
}

void TextureCache::applySampler(QOpenGLFunctions_2_0& gl, const EmitterState& state, s32 maxMipLevel) const {
    const s32 requestedLevel = (state.textureLodLevel > 0)
        ? static_cast<s32>(state.textureLodLevel) - 1
        : 0;

    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(minFilterOf(state.textureFilter)));
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(magFilterOf(state.textureFilter)));
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     static_cast<GLint>(wrapModeOf(state.textureWrapS)));
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     static_cast<GLint>(wrapModeOf(state.textureWrapT)));
    gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,  static_cast<GLint>(std::min(requestedLevel, maxMipLevel)));
}

TextureBinding TextureCache::acquire(QOpenGLFunctions_2_0& gl, const EmitterState& state) {
    TextureBinding binding{};

    if (state.textureHandle == nullptr || !state.textureHandle->isValid()) {
        return binding;
    }

    const auto* texture = state.textureHandle->get();
    if (texture == nullptr || texture->isPlaceholder()) {
        return binding;
    }

    const auto& source = texture->textureData();
    if (source.isNull()) {
        return binding;
    }

    binding.texelWidth = 1.0f / static_cast<f32>(source.width());
    binding.texelHeight = 1.0f / static_cast<f32>(source.height());

    const u64 imageKey = static_cast<u64>(source.cacheKey());
    const auto cached = mTextures.find(texture);
    u32 id = 0;
    if (cached != mTextures.end() && cached->second.imageKey == imageKey) {
        id = cached->second.id;
    } else {
        if (cached != mTextures.end()) {
            const u32 staleId = cached->second.id;
            gl.glDeleteTextures(1, &staleId);
            mTextures.erase(cached);
        }

        id = upload(gl, source);
        if (id != 0) {
            mTextures.emplace(texture, CachedTexture{id, imageKey});
        }
    }
    if (id == 0) {
        return binding;
    }

    const auto maxDimension = static_cast<u32>(std::max(source.width(), source.height()));
    const s32 maxMipLevel = static_cast<s32>(std::bit_width(maxDimension)) - 1;

    binding.hasTexture = true;

    gl.glBindTexture(GL_TEXTURE_2D, id);
    applySampler(gl, state, maxMipLevel);
    return binding;
}

void TextureCache::clear(QOpenGLFunctions_2_0& gl) {
    for (const auto& entry : mTextures) {
        const u32 id = entry.second.id;
        gl.glDeleteTextures(1, &id);
    }
    mTextures.clear();
}


// ========================================================================== //


} // namespace Ptcl::Render
