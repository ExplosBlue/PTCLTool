#include "editor/components/texturePreview.h"

#include "util/paintUtil.h"

#include <QApplication>
#include <QGuiApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QGroupBox>


namespace PtclEditor {


// ========================================================================== //


TexturePreview::TexturePreview(QWidget* parent) :
    QWidget{parent} {

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setMinimumSize(128, 128);
}

QSize TexturePreview::sizeHint() const {
    return {256, 256};
}

s32 TexturePreview::heightForWidth(s32 w) const {
    if (mTexture.isNull()) {
        return w / 2;
    }
    const f32 aspect = static_cast<f32>(mTexture.width()) * static_cast<f32>(mDivX) / (static_cast<f32>(mTexture.height()) * static_cast<f32>(mDivY));

    return std::max(64, static_cast<s32>(std::round(static_cast<f32>(w) / aspect)));
}

void TexturePreview::setSource(const QImage& image, s32 divX, s32 divY) {
    mTexture = image;

    mDivX = std::max(1, divX);
    mDivY = std::max(1, divY);


    clampDivisions();

    updateLayoutCache();
    rebuildTexture();
    update();
}

void TexturePreview::setRepetitions(s32 repX, s32 repY) {
    repX = std::max(1, repX);
    repY = std::max(1, repY);

    if (mRepX == repX && mRepY == repY) {
        return;
    }

    mRepX = repX;
    mRepY = repY;

    updateLayoutCache();
    rebuildTexture();
    updateGeometry();
    update();
}

void TexturePreview::setDivisions(s32 divX, s32 divY) {
    mDivX = std::max(1, divX);
    mDivY = std::max(1, divY);

    clampDivisions();
    update();
}

void TexturePreview::clampDivisions() {
    while (mDivX * mDivY > 16) {
        if (mDivX > mDivY) {
            --mDivX;
        } else {
            --mDivY;
        }
    }
}

void TexturePreview::setWrapModes(Ptcl::TextureWrap wrapU, Ptcl::TextureWrap wrapV) {
    if (mWrapU == wrapU && mWrapV == wrapV) {
        return;
    }

    mWrapU = wrapU;
    mWrapV = wrapV;

    rebuildTexture();
    update();
}

void TexturePreview::setFilter(Ptcl::TextureFilter filter) {
    if (mFilter == filter) {
        return;
    }

    mFilter = filter;

    rebuildTexture();
    update();
}

void TexturePreview::showFrameNumbers(bool show) {
    mShowFrameNumbers = show;
    update();
}

void TexturePreview::updateLayoutCache() {
    if (mTexture.isNull()) {
        mTexRect = {};
        return;
    }

    QSizeF virtualSize{
        static_cast<f32>(mTexture.width()) * static_cast<f32>(mRepX),
        static_cast<f32>(mTexture.height()) * static_cast<f32>(mRepY)
    };

    const s32 availW = std::max(0, width() - padding * 2);
    const s32 availH = std::max(0, height() - padding * 2);

    if (availW <= 0 || availH <= 0) {
        mTexRect = {};
        return;
    }

    mScale = std::min(
        static_cast<f32>(availW) / static_cast<f32>(virtualSize.width()),
        static_cast<f32>(availH) / static_cast<f32>(virtualSize.height())
    );

    const QSize scaledSize{
        qRound(virtualSize.width() * mScale),
        qRound(virtualSize.height() * mScale)
    };

    const QPoint topLeft{
        (width() - scaledSize.width()) / 2,
        (height() - scaledSize.height()) / 2
    };

    mTexRect = QRect(topLeft, scaledSize);
}

void TexturePreview::rebuildTexture() {
    if (mTexture.isNull()) {
        mRenderedTexture = {};
        return;
    }

    const QSize sourceSize{
        mTexture.width() * mRepX,
        mTexture.height() * mRepY
    };

    mRenderedTexture = QImage{mTexRect.size(), QImage::Format_ARGB32_Premultiplied};
    mRenderedTexture.fill(Qt::transparent);

    QPainter painter(&mRenderedTexture);
    PaintUtil::drawCheckerboard(painter, mRenderedTexture.rect(), 2, sourceSize);

    const QImage texture = PaintUtil::renderTexture(
        mTexture,
        QRectF{0.0, 0.0, static_cast<f32>(mRepX), static_cast<f32>(mRepY)},
        mWrapU,
        mWrapV,
        mFilter,
        mTexRect.size()
    );
    painter.drawImage(0, 0, texture);
}

QRect TexturePreview::frameRegionRect(s32 frameIndex) const {
    const s32 fx = frameIndex % mDivX;
    const s32 fy = frameIndex / mDivX;

    const s32 left = mTexRect.left() + mTexRect.width() * fx / mDivX;
    const s32 right = mTexRect.left() + mTexRect.width() * (fx + 1) / mDivX;

    const s32 top = mTexRect.top() + mTexRect.height() * fy / mDivY;
    const s32 bottom = mTexRect.top() + mTexRect.height() * (fy + 1) / mDivY;

    return {
        left,
        top,
        right - left,
        bottom - top
    };
}

void TexturePreview::drawTexture(QPainter& painter) {
    painter.save();
    painter.setRenderHint(QPainter::SmoothPixmapTransform, false);
    painter.drawImage(mTexRect, mRenderedTexture);
    painter.restore();
}

void TexturePreview::drawFrameRegions(QPainter& painter) {
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);

    const QPen darkPen(QColor(0, 0, 0, 160), 1);
    const QPen lightPen(QColor(255, 255, 255, 160), 1);

    const s32 left = mTexRect.left();
    const s32 top = mTexRect.top();
    const s32 endX = mTexRect.left() + mTexRect.width();
    const s32 endY = mTexRect.top() + mTexRect.height();

    const s32 cellW = mTexRect.width() / mDivX;
    const s32 cellH = mTexRect.height() / mDivY;

    for (s32 i = 1; i < mDivX; ++i) {
        const s32 x = left + i * cellW;
        painter.setPen(darkPen);
        painter.drawLine(x, top, x, endY);
        painter.setPen(lightPen);
        painter.drawLine(x + 1, top, x + 1, endY);
    }

    for (s32 i = 1; i < mDivY; ++i) {
        const s32 y = top + i * cellH;
        painter.setPen(darkPen);
        painter.drawLine(left, y, endX, y);
        painter.setPen(lightPen);
        painter.drawLine(left, y + 1, endX, y + 1);
    }

    painter.restore();
}

void TexturePreview::drawFrameNumbers(QPainter& painter) {
    const s32 totalFrames = mDivX * mDivY;

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QFont font = painter.font();
    const s32 cellW = mTexRect.width() / mDivX;
    const s32 cellH = mTexRect.height() / mDivY;
    const s32 fontSize = std::clamp(std::min(cellW, cellH) / 5, 8, 18);

    QFont numFont(font.family(), fontSize);
    numFont.setBold(true);
    painter.setFont(numFont);

    const QFontMetrics fm(numFont);
    const s32 badgeW = fm.maxWidth() + 6;
    const s32 badgeH = fm.height() + 2;

    const QColor bgColor = palette().color(QPalette::Shadow);

    for (s32 i = 0; i < totalFrames; ++i) {
        const QRect rect = frameRegionRect(i);
        const QString text = QString::number(i);

        const QRect bgRect(rect.topLeft(), QSize(badgeW, badgeH));

        QColor bg = bgColor;
        bg.setAlpha(120);

        painter.setPen(Qt::NoPen);
        painter.setBrush(bg);
        painter.drawRoundedRect(bgRect, 2, 2);

        painter.setPen(palette().color(QPalette::Text));
        painter.drawText(bgRect, Qt::AlignCenter, text);
    }

    painter.restore();
}

void TexturePreview::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), palette().color(QPalette::Base));

    if (mRenderedTexture.isNull()) {
        return;
    }

    drawTexture(painter);
    drawFrameRegions(painter);

    if (mShowFrameNumbers) {
        drawFrameNumbers(painter);
    }
}

void TexturePreview::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    updateLayoutCache();
    update();
}


// ========================================================================== //


} // namespace PtclEditor
