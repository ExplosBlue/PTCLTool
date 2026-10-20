#pragma once

#include <array>

#include <QOpenGLFunctions_2_0>

#include "ptcl/ptclEnum.h"
#include "typedefs.h"


namespace Ptcl::Render {


// ========================================================================== //


inline constexpr s32 cCombinerStageCount = 4;
inline constexpr s32 cCombinerStageStride = 8;
inline constexpr s32 cCombinerConfigSize = cCombinerStageCount * cCombinerStageStride;

using CombinerConfig = std::array<s32, cCombinerConfigSize>;

CombinerConfig packCombinerConfig(ColorCombinerFuncType function);

void setupBlendState(QOpenGLFunctions_2_0& gl, BlendFuncType blendFunction);
void setupDepthState(QOpenGLFunctions_2_0& gl, DepthFuncType depthFunction);
void setupRenderState(QOpenGLFunctions_2_0& gl, BlendFuncType blendFunction, DepthFuncType depthFunction);

GLenum wrapModeOf(TextureWrap wrap);
GLenum minFilterOf(TextureFilter filter);
GLenum magFilterOf(TextureFilter filter);


// ========================================================================== //


} // namespace Ptcl::Render
