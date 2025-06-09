#include "editor/colorGradientEditor.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>


// ========================================================================== //


ColorGradientEditor::ColorGradientEditor(QWidget* parent) :
    QWidget{parent} {
    setMouseTracking(true);
}

void ColorGradientEditor::setTimings(s32 inCompleted, s32 peak, s32 outStart) {

    f32 inCompletedPercent = static_cast<f32>(inCompleted) / 100.0f;
    f32 peakPercent = static_cast<f32>(peak) / 100.0f;
    f32 outStartPercent = static_cast<f32>(outStart) / 100.0f;

    mInCompleted = inCompletedPercent;
    mPeak = peakPercent;
    mOutStart = outStartPercent;
    update();
}

void ColorGradientEditor::setColors(const QColor& initial, const QColor& peak, const QColor& end) {
    mInitialColor = initial;
    mPeakColor = peak;
    mEndColor = end;
    update();
}

void ColorGradientEditor::setRepetitionCount(int repetitions) {
    mRepetitions = repetitions;
    update();
}

s32 ColorGradientEditor::inCompletedTiming() const {
    return static_cast<s32>(mInCompleted * 100.0f);
}

s32 ColorGradientEditor::peakTiming() const {
    return static_cast<s32>(mPeak * 100.0f);
}

s32 ColorGradientEditor::outStartTiming() const {
    return static_cast<s32>(mOutStart * 100.0f);
}

void ColorGradientEditor::setInitialColor(const QColor& color) {
    mInitialColor = color;
    update();
}

void ColorGradientEditor::setPeakColor(const QColor& color) {
    mPeakColor = color;
    update();
}

void ColorGradientEditor::setEndColor(const QColor& color) {
    mEndColor = color;
    update();
}

void ColorGradientEditor::drawGradientBar(QPainter& painter, const QRectF& rect, int repetitions) {
    if (repetitions <= 0) {
        return;
    }

    // Create an image for one repetition
    QImage singleGradient(qRound(rect.width() / repetitions), qRound(rect.height()), QImage::Format_RGB32);

    for (int x = 0; x < singleGradient.width(); ++x) {
        f32 pos = static_cast<f32>(x) / static_cast<f32>(singleGradient.width());
        QColor color;

        if (pos <= mInCompleted) {
            color = mInitialColor;
        } else if (pos <= mPeak) {
            f32 t = (pos - mInCompleted) / (mPeak - mInCompleted);
            color = QColor::fromRgbF(
                mInitialColor.redF()   * (1 - t) + mPeakColor.redF()   * t,
                mInitialColor.greenF() * (1 - t) + mPeakColor.greenF() * t,
                mInitialColor.blueF()  * (1 - t) + mPeakColor.blueF()  * t
                );
        } else if (pos <= mOutStart) {
            f32 t = (pos - mPeak) / (mOutStart - mPeak);
            color = QColor::fromRgbF(
                mPeakColor.redF()   * (1 - t) + mEndColor.redF()   * t,
                mPeakColor.greenF() * (1 - t) + mEndColor.greenF() * t,
                mPeakColor.blueF()  * (1 - t) + mEndColor.blueF()  * t
                );
        } else {
            color = mEndColor;
        }

        for (int y = 0; y < singleGradient.height(); ++y) {
            singleGradient.setPixelColor(x, y, color);
        }
    }

    QPainterPath barMask;
    barMask.addRoundedRect(rect, rect.height() / 2.0, rect.height() / 2.0);

    painter.save();
    painter.setClipPath(barMask);

    // Tile the singleGradient horizontally repetitions times inside rect
    for (int i = 0; i < repetitions; ++i) {
        QRect targetRect(
            qRound(rect.left() + i * singleGradient.width()),
            qRound(rect.top()),
            singleGradient.width(),
            singleGradient.height());
        painter.drawImage(targetRect, singleGradient);
    }

    painter.restore();
}

f32 ColorGradientEditor::calcGradientBarHeight() const {
    QFontMetrics fm(font());
    const int fontHeight = fm.height();
    return static_cast<f32>(fontHeight) / 1.5f;
}

f32 ColorGradientEditor::calcHandleHeight() const {
    QFontMetrics fm(font());
    const int fontHeight = fm.height();
    return static_cast<f32>(fontHeight) / 2.0f;
}

f32 ColorGradientEditor::calcHandleWidth() const {
    QFontMetrics fm(font());
    const int fontHeight = fm.height();
    return static_cast<f32>(fontHeight) / 1.5f;
}

f32 ColorGradientEditor::calcTextHeight() const {
    QFontMetrics fm(font());
    const int fontHeight = fm.height();
    return static_cast<f32>(fontHeight);
}

void ColorGradientEditor::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Compute inner drawable width
    const f32 innerWidth = static_cast<f32>(width()) - 2 * sMargin;

    // Colors
    auto handleColor = palette().color(QPalette::Text);
    auto borderColor = palette().color(QPalette::Dark);
    auto textColor = palette().color(QPalette::Text);

    // ----------------------------------------------------

    // Top gradient bar area inside margins
    const f32 barHeight = calcGradientBarHeight();
    QRectF gradientRect(sMargin, 0, innerWidth, barHeight);

    drawGradientBar(painter, gradientRect, 1);

    // ----------------------------------------------------

    // Handle line Y positions
    const f32 lineTopY = static_cast<f32>(gradientRect.top());
    const f32 lineBottomY = lineTopY + barHeight;

    // Draw handle lines over gradient, with margin offset
    painter.setPen(QPen(borderColor, 1));
    for (const auto& timing : {mInCompleted, mPeak, mOutStart}) {
        f32 x = sMargin + timing * innerWidth;
        QLineF line(x, lineTopY, x, lineBottomY);
        painter.drawLine(line);
    }

    // ----------------------------------------------------

    // Handles
    std::array<std::pair<f32, QString>, 3> handles {{
        {mInCompleted, "In"},
        {mPeak, "Peak"},
        {mOutStart, "Out"}
    }};

    const f32 handleHeight = calcHandleHeight();
    const f32 handleWidth = calcHandleWidth();

    f32 handleTopY = lineBottomY;
    f32 handleBottomY = handleTopY + handleHeight;

    painter.setBrush(handleColor);
    painter.setPen(QPen(borderColor, 1));
    for (const auto& handle : handles) {
        f32 x = sMargin + handle.first * innerWidth;
        std::array<QPointF, 3> points = {
            QPointF(x, handleTopY),
            QPointF(x - handleWidth / 2, handleBottomY),
            QPointF(x + handleWidth / 2, handleBottomY)
        };
        painter.drawPolygon(points.data(), 3);
    }

    // ----------------------------------------------------

    const f32 labelTopY = handleBottomY;
    const f32 lineHeight = calcTextHeight();

    painter.save();
    QFont font = painter.font();
    font.setPointSize(8);
    painter.setFont(font);
    painter.setPen(QPen(textColor, 1));
    QFontMetrics fm(painter.font());

    for (const auto& handle : handles) {
        f32 x = sMargin + handle.first * innerWidth;
        QString text = QString("%1 (%2%)").arg(handle.second).arg(static_cast<int>(handle.first * 100));

        int textWidth = fm.horizontalAdvance(text);
        f32 labelWidth = static_cast<f32>(textWidth);
        f32 labelX = std::clamp(x - labelWidth / 2.0f, sMargin, sMargin + innerWidth - labelWidth);

        QRectF labelRect(labelX, labelTopY, labelWidth, lineHeight);
        painter.drawText(labelRect, Qt::AlignCenter, text);
    }
    painter.restore();

    // ----------------------------------------------------

    const f32 repetitionsBarY = labelTopY + lineHeight;
    QRectF repetitionsRect(sMargin, repetitionsBarY, innerWidth, barHeight);
    drawGradientBar(painter, repetitionsRect, mRepetitions);
}

void ColorGradientEditor::mousePressEvent(QMouseEvent* event) {
    const f32 w = static_cast<f32>(width());
    const f32 innerWidth = w - 2 * sMargin;

    const f32 x = static_cast<f32>(event->position().x());

    std::array<f32, 3> positions = {
        sMargin + mInCompleted * innerWidth,
        sMargin + mPeak * innerWidth,
        sMargin + mOutStart * innerWidth
    };

    for (int i = 0; i < positions.size(); ++i) {
        if (std::abs(x - positions[i]) < 6) {
            mActiveHandle = i;
            return;
        }
    }

    QWidget::mousePressEvent(event);
}

void ColorGradientEditor::mouseMoveEvent(QMouseEvent* event) {
    const f32 w = static_cast<f32>(width());
    const f32 innerWidth = w - 2 * sMargin;

    const f32 x = static_cast<f32>(event->position().x());

    if (mActiveHandle != -1) {
        f32 clampedX = std::clamp((x - sMargin) / innerWidth, 0.0f, 1.0f);

        auto handleType = static_cast<HandleType>(mActiveHandle);

        switch (handleType) {
        case HandleType::InCompletedHandle:
            mInCompleted = clampedX;
            break;
        case HandleType::PeakHandle:
            mPeak = clampedX;
            break;
        case HandleType::OutStartHandle:
            mOutStart = clampedX;
            break;
        }

        emit handleMoved(handleType);
        update();
    }

    // Always check cursor hover state, even if dragging
    bool hoveringHandle = false;
    std::array<f32, 3> positions = {
        sMargin + mInCompleted * innerWidth,
        sMargin + mPeak * innerWidth,
        sMargin + mOutStart * innerWidth
    };

    for (f32 handleX : positions) {
        if (std::abs(x - handleX) < 6.0f) {
            hoveringHandle = true;
            break;
        }
    }

    setCursor(hoveringHandle || mActiveHandle != -1 ? Qt::PointingHandCursor : Qt::ArrowCursor);

    QWidget::mouseMoveEvent(event);
}

void ColorGradientEditor::mouseReleaseEvent(QMouseEvent* event) {
    mActiveHandle = -1;
    QWidget::mouseReleaseEvent(event);
}

void ColorGradientEditor::leaveEvent(QEvent* event) {
    setCursor(Qt::ArrowCursor);
    QWidget::leaveEvent(event);
}

QSize ColorGradientEditor::sizeHint() const {
    const int defaultWidth = 300;

    const f32 barHeight = calcGradientBarHeight();
    const f32 handleHeight = calcHandleHeight();
    const f32 textHeight = calcTextHeight();

    f32 totalHeight = barHeight     // top bar
                      + handleHeight
                      + textHeight
                      + barHeight;  // repetitions bar

    return {defaultWidth, static_cast<int>(totalHeight)};
}

QSize ColorGradientEditor::minimumSizeHint() const {
    return {100, sizeHint().height()};
}


// ========================================================================== //
