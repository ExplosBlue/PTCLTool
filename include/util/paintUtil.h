#pragma once

#include "typedefs.h"

#include <QColor>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QRect>


// ========================================================================== //


struct TextureFrameInfo {
    const QImage* texture = nullptr;
    s32 divX = 1;
    s32 divY = 1;
    s32 repX = 1;
    s32 repY = 1;
};


// ========================================================================== //


namespace PaintUtil {


// ========================================================================== //


f32 mirrorTextureCoord(f32 t);

QPixmap renderTextureFrame(const TextureFrameInfo& frameInfo, s32 frame, s32 size);

void drawCheckerboard(QPainter& painter, const QRect& rect, s32 checkerSize = 8, const QColor& light = {220, 220, 220}, const QColor& dark = {180, 180, 180});


// ========================================================================== //


} // namespace PaintUtil
