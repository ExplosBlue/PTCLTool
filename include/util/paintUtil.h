#pragma once

#include "typedefs.h"

#include <QColor>
#include <QPainter>
#include <QRect>


namespace PaintUtil {


// ========================================================================== //


void drawCheckerboard(QPainter& painter, const QRect& rect, s32 checkerSize = 8, const QColor& light = {220, 220, 220}, const QColor& dark = {180, 180, 180});


// ========================================================================== //


} // namespace PaintUtil
