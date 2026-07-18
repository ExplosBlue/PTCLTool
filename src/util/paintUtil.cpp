#include "util/paintUtil.h"


namespace PaintUtil {


// ========================================================================== //

f32 mirrorTextureCoord(f32 t) {
    const s32 i = static_cast<s32>(std::floor(t));
    const f32 f = t - static_cast<f32>(i);
    return (i % 2 == 0) ? f : (1.0f - f);
}

f32 clampToEdgeCoord(f32 t) {
    return std::clamp(t, 0.0f, 1.0f);
}

f32 repeatCoord(f32 t) {
    return t - std::floor(t);
}

f32 wrapTextureCoord(f32 coord, Ptcl::TextureWrap wrap) {
    switch (wrap) {
        case Ptcl::TextureWrap::MirroredRepeat: return mirrorTextureCoord(coord);
        case Ptcl::TextureWrap::Repeat:         return repeatCoord(coord);
        case Ptcl::TextureWrap::ClampToEdge:    return clampToEdgeCoord(coord);
        default: std::unreachable();
    }
    return coord;
}

QColor sampleTexture(const QImage& texture, f32 u, f32 v, Ptcl::TextureFilter filter) {
    const f32 texW = static_cast<f32>(texture.width());
    const f32 texH = static_cast<f32>(texture.height());

    if (filter == Ptcl::TextureFilter::Nearest) {
        const s32 tx = std::clamp(static_cast<s32>(std::floor(u * texW)), 0, texture.width() - 1);
        const s32 ty = std::clamp(static_cast<s32>(std::floor(v * texH)), 0, texture.height() - 1);
        return texture.pixelColor(tx, ty);
    }

    const f32 x = std::clamp(u * texW - 0.5f, 0.0f, texW - 1.0f);
    const f32 y = std::clamp(v * texH - 0.5f, 0.0f, texH - 1.0f);

    const s32 x0 = static_cast<s32>(std::floor(x));
    const s32 y0 = static_cast<s32>(std::floor(y));

    const s32 x1 = std::min(x0 + 1, texture.width() - 1);
    const s32 y1 = std::min(y0 + 1, texture.height() - 1);

    const f32 fx = x - static_cast<f32>(x0);
    const f32 fy = y - static_cast<f32>(y0);

    const QColor c00 = texture.pixelColor(x0, y0);
    const QColor c10 = texture.pixelColor(x1, y0);
    const QColor c01 = texture.pixelColor(x0, y1);
    const QColor c11 = texture.pixelColor(x1, y1);

    const f32 topR = std::lerp(c00.redF(), c10.redF(), fx);
    const f32 topG = std::lerp(c00.greenF(), c10.greenF(), fx);
    const f32 topB = std::lerp(c00.blueF(), c10.blueF(), fx);
    const f32 topA = std::lerp(c00.alphaF(), c10.alphaF(), fx);

    const f32 botR = std::lerp(c01.redF(), c11.redF(), fx);
    const f32 botG = std::lerp(c01.greenF(), c11.greenF(), fx);
    const f32 botB = std::lerp(c01.blueF(), c11.blueF(), fx);
    const f32 botA = std::lerp(c01.alphaF(), c11.alphaF(), fx);

    return QColor::fromRgbF(
        std::lerp(topR, botR, fy),
        std::lerp(topG, botG, fy),
        std::lerp(topB, botB, fy),
        std::lerp(topA, botA, fy)
    );
}


QPixmap renderTextureFrame(const TextureFrameInfo& frameInfo, s32 frame, s32 size) {
    if (!frameInfo.texture || frameInfo.texture->isNull() || frameInfo.divX <= 0 || frameInfo.divY <= 0 || frameInfo.repX <= 0 || frameInfo.repY <= 0) {
        QPixmap pm(size, size);
        pm.fill(Qt::darkGray);
        return pm;
    }

    const s32 fx = frame % frameInfo.divX;
    const s32 fy = frame / frameInfo.divX;

    const f32 frameWidth = static_cast<f32>(frameInfo.repX) / static_cast<f32>(frameInfo.divX);
    const f32 frameHeight = static_cast<f32>(frameInfo.repY) / static_cast<f32>(frameInfo.divY);

    const QRectF uvRect{
        static_cast<f32>(fx) * frameWidth,
        static_cast<f32>(fy) * frameHeight,
        frameWidth,
        frameHeight
    };

    auto tex = renderTexture(
        *frameInfo.texture,
        uvRect,
        frameInfo.wrapU,
        frameInfo.wrapV,
        frameInfo.filter,
        {size, size}
    );

    return QPixmap::fromImage(tex);
}

QImage renderTexture(const QImage& texture, const QRectF& uvRect, Ptcl::TextureWrap wrapU, Ptcl::TextureWrap wrapV, Ptcl::TextureFilter filter, const QSize& size) {
    QImage out(size, QImage::Format_ARGB32);

    for (s32 y = 0; y < size.height(); ++y) {
        for (s32 x = 0; x < size.width(); ++x) {
            const f32 u = static_cast<f32>(uvRect.left()) + (static_cast<f32>(x) + 0.5f) / static_cast<f32>(size.width()) * static_cast<f32>(uvRect.width());
            const f32 v = static_cast<f32>(uvRect.top()) + (static_cast<f32>(y) + 0.5f) / static_cast<f32>(size.height()) * static_cast<f32>(uvRect.height());

            const f32 wrappedU = wrapTextureCoord(u, wrapU);
            const f32 wrappedV = wrapTextureCoord(v, wrapV);

            out.setPixelColor(x, y, sampleTexture(texture, wrappedU, wrappedV, filter));
        }
    }
    return out;
}

void drawCheckerboard(QPainter& painter, const QRect& rect, s32 checkerSize, const QSize& sourceSize, const QColor& light, const QColor& dark) {
    if (checkerSize <= 0 || sourceSize.isEmpty() || rect.isEmpty()) {
        return;
    }

    const f32 scaleX = static_cast<f32>(rect.width()) / static_cast<f32>(sourceSize.width());
    const f32 scaleY = static_cast<f32>(rect.height()) / static_cast<f32>(sourceSize.height());

    const s32 checkerCountX = (sourceSize.width() + checkerSize - 1) / checkerSize;
    const s32 checkerCountY = (sourceSize.height() + checkerSize - 1) / checkerSize;

    painter.save();

    for (s32 cy = 0; cy < checkerCountY; ++cy) {
        for (s32 cx = 0; cx < checkerCountX; ++cx) {
            const s32 sourceX = cx * checkerSize;
            const s32 sourceY = cy * checkerSize;

            const s32 sourceW = std::min(checkerSize, sourceSize.width() - sourceX);
            const s32 sourceH = std::min(checkerSize, sourceSize.height() - sourceY);

            const QRectF checkerRect{
                static_cast<f32>(rect.left() + sourceX) * scaleX,
                static_cast<f32>(rect.top() + sourceY) * scaleY,
                static_cast<f32>(sourceW) * scaleX,
                static_cast<f32>(sourceH) * scaleY
            };

            const bool isLight = ((cx + cy) & 1) == 0;

            painter.fillRect(checkerRect, isLight ? light : dark);
        }
    }

    painter.restore();
}


// ========================================================================== //


} // namespace PaintUtil
