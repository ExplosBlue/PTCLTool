#include "util/paintUtil.h"


namespace PaintUtil {


// ========================================================================== //


void drawCheckerboard(QPainter& painter, const QRect& rect, s32 checkerSize, const QColor& light, const QColor& dark) {
    if (checkerSize <= 0 || rect.isEmpty()) {
        return;
    }

    painter.save();

    const s32 left = rect.left();
    const s32 right = rect.right();
    const s32 top = rect.top();
    const s32 bottom = rect.bottom();

    for (s32 y = top; y < bottom; y += checkerSize) {
        for (s32 x = left; x < right; x += checkerSize) {
            const s32 localX = x - left;
            const s32 localY = y - top;

            bool isLight = ((localX / checkerSize + localY / checkerSize) & 1) == 0;

            painter.fillRect(QRect{x, y, checkerSize, checkerSize}, isLight ? light : dark);
        }
    }

    painter.restore();
}


// ========================================================================== //


} // namespace PaintUtil
