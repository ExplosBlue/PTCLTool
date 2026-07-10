#include "util/paintUtil.h"


namespace PaintUtil {


// ========================================================================== //

f32 mirrorTextureCoord(f32 t) {
    const s32 i = static_cast<s32>(std::floor(t));
    const f32 f = t - static_cast<f32>(i);
    return (i % 2 == 0) ? f : (1.0f - f);
}


QPixmap renderTextureFrame(const TextureFrameInfo& frameInfo, s32 frame, s32 size) {
    if (!frameInfo.texture || frameInfo.texture->isNull() || frameInfo.divX <= 0 || frameInfo.divY <= 0) {
        QPixmap pm(size, size);
        pm.fill(Qt::darkGray);
        return pm;
    }

    const auto& texture = *frameInfo.texture;
    const s32 fx = frame % frameInfo.divX;
    const s32 fy = frame / frameInfo.divX;

    const f32 tilesPerFrameX = static_cast<f32>(frameInfo.repX) / static_cast<f32>(frameInfo.divX);
    const f32 tilesPerFrameY = static_cast<f32>(frameInfo.repY) / static_cast<f32>(frameInfo.divY);

    const f32 tilesStartX = static_cast<f32>(fx) * tilesPerFrameX;
    const f32 tilesStartY = static_cast<f32>(fy) * tilesPerFrameY;

    QImage out(size, size, QImage::Format_ARGB32);

    for (s32 y = 0; y < out.height(); ++y) {
        for (s32 x = 0; x < out.width(); ++x) {
            f32 tileU = tilesStartX + (static_cast<f32>(x) / static_cast<f32>(out.width())) * tilesPerFrameX;
            f32 tileV = tilesStartY + (static_cast<f32>(y) / static_cast<f32>(out.height())) * tilesPerFrameY;

            f32 u = tileU / static_cast<f32>(frameInfo.repX);
            f32 v = tileV / static_cast<f32>(frameInfo.repY);

            f32 mu = mirrorTextureCoord(u * static_cast<f32>(frameInfo.repX));
            f32 mv = mirrorTextureCoord(v * static_cast<f32>(frameInfo.repY));

            s32 tx = std::clamp(static_cast<s32>(mu * static_cast<f32>(texture.width())), 0, texture.width() - 1);
            s32 ty = std::clamp(static_cast<s32>(mv * static_cast<f32>(texture.height())), 0, texture.height() - 1);

            out.setPixelColor(x, y, texture.pixelColor(tx, ty));
        }
    }
    return QPixmap::fromImage(out);
}

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
