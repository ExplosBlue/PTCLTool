#pragma once

#include "typedefs.h"

#include "ptcl/ptclEnum.h"

#include <QColor>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QRect>


// ========================================================================== //


struct TextureFrameInfo {
    const QImage* texture = nullptr;
    Ptcl::TextureWrap wrapU{Ptcl::TextureWrap::MirroredRepeat};
    Ptcl::TextureWrap wrapV{Ptcl::TextureWrap::MirroredRepeat};
    Ptcl::TextureFilter filter{Ptcl::TextureFilter::Nearest};
    s32 divX = 1;
    s32 divY = 1;
    s32 repX = 1;
    s32 repY = 1;
};


// ========================================================================== //


namespace PaintUtil {


// ========================================================================== //


f32 mirrorTextureCoord(f32 t);
f32 clampToEdgeCoord(f32 t);
f32 repeatCoord(f32 t);

f32 wrapTextureCoord(f32 coord, Ptcl::TextureWrap wrap);

QColor sampleTexture(const QImage& texture, f32 u, f32 v, Ptcl::TextureFilter filter);

QPixmap renderTextureFrame(const TextureFrameInfo& frameInfo, s32 frame, s32 size);

QImage renderTexture(const QImage& texture, const QRectF& uvRect, Ptcl::TextureWrap wrapU, Ptcl::TextureWrap wrapV, Ptcl::TextureFilter filter, const QSize& size);

void drawCheckerboard(QPainter& painter, const QRect& rect, s32 checkerSize, const QSize& sourceSize, const QColor& light = {220, 220, 220}, const QColor& dark = {180, 180, 180});


// ========================================================================== //


} // namespace PaintUtil
