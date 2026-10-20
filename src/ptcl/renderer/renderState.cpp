#include "ptcl/renderer/renderState.h"

#include <array>
#include <cstddef>
#include <utility>

#include <QOpenGLFunctions_2_0>

#include "ptcl/renderer/combiner.h"


namespace Ptcl::Render {


// ========================================================================== //


CombinerConfig packCombinerConfig(ColorCombinerFuncType function) {
    CombinerConfig config{};

    const auto index = static_cast<std::size_t>(function);
    if (index >= Combiner::configurations.size()) {
        return config;
    }

    const Combiner::Config& combiner = Combiner::configurations[index];
    const std::array<const Combiner::Stage*, cCombinerStageCount> stages = {
        &combiner.colorStage[0], &combiner.colorStage[1],
        &combiner.alphaStage[0], &combiner.alphaStage[1],
    };

    for (s32 stageIndex = 0; stageIndex < cCombinerStageCount; ++stageIndex) {
        const Combiner::Stage& stage = *stages[static_cast<std::size_t>(stageIndex)];
        const s32 base = stageIndex * cCombinerStageStride;

        config[base + 0] = static_cast<s32>(stage.func);
        config[base + 1] = static_cast<s32>(stage.inputs[0]);
        config[base + 2] = static_cast<s32>(stage.inputs[1]);
        config[base + 3] = static_cast<s32>(stage.inputs[2]);
        config[base + 4] = static_cast<s32>(stage.operands[0]);
        config[base + 5] = static_cast<s32>(stage.operands[1]);
        config[base + 6] = static_cast<s32>(stage.operands[2]);
        config[base + 7] = static_cast<s32>(stage.scale);
    }

    return config;
}

void setupBlendState(QOpenGLFunctions_2_0& gl, BlendFuncType blendFunction) {
    switch (blendFunction) {
    case BlendFuncType::AlphaBlend:
        gl.glBlendEquation(GL_FUNC_ADD);
        gl.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        break;
    case BlendFuncType::Additive:
        gl.glBlendEquation(GL_FUNC_ADD);
        gl.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
        break;
    case BlendFuncType::Subtractive:
        gl.glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
        gl.glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_SRC_ALPHA, GL_ONE);
        break;
    case BlendFuncType::Opaque:
        gl.glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_REVERSE_SUBTRACT);
        gl.glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ZERO);
        break;
    }
}

void setupDepthState(QOpenGLFunctions_2_0& gl, DepthFuncType depthFunction) {
    switch (depthFunction) {
    case DepthFuncType::TestOnly:
        gl.glEnable(GL_DEPTH_TEST);
        gl.glDepthFunc(GL_LESS);
        gl.glDepthMask(GL_FALSE);
        gl.glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        break;
    case DepthFuncType::Disabled:
        gl.glDisable(GL_DEPTH_TEST);
        gl.glDepthMask(GL_FALSE);
        gl.glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        break;
    case DepthFuncType::TestAndWrite:
        gl.glEnable(GL_DEPTH_TEST);
        gl.glDepthFunc(GL_LESS);
        gl.glDepthMask(GL_TRUE);
        gl.glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        break;
    }
}

void setupRenderState(QOpenGLFunctions_2_0& gl, BlendFuncType blendFunction,
    DepthFuncType depthFunction) {
    setupBlendState(gl, blendFunction);
    setupDepthState(gl, depthFunction);
}

GLenum wrapModeOf(TextureWrap wrap) {
    switch (wrap) {
    case TextureWrap::Repeat:
        return GL_REPEAT;
    case TextureWrap::MirroredRepeat:
        return GL_MIRRORED_REPEAT;
    case TextureWrap::ClampToEdge:
        return GL_CLAMP_TO_EDGE;
    default:
        std::unreachable();
    }
}

GLenum minFilterOf(TextureFilter filter) {
    return (filter == TextureFilter::Nearest) ? GL_NEAREST : GL_NEAREST_MIPMAP_LINEAR;
}

GLenum magFilterOf(TextureFilter filter) {
    return (filter == TextureFilter::Nearest) ? GL_NEAREST : GL_LINEAR;
}


// ========================================================================== //


} // namespace Ptcl::Render
