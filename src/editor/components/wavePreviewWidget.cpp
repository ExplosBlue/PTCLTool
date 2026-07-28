#include "editor/components/wavePreviewWidget.h"

#include <QPainter>
#include <QtMath>

#include <numbers>

// ==========================================================================//


WavePreviewWidget::WavePreviewWidget(QWidget* parent) :
    QWidget{parent} {
    setMinimumSize(200, 120);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void WavePreviewWidget::setAmplitude(f32 amplitude) {
    if (mAmplitude != amplitude) {
        mAmplitude = amplitude;
        update();
    }
}

void WavePreviewWidget::setFrequency(f32 frequency) {
    if (mFrequency != frequency) {
        mFrequency = frequency;
        update();
    }
}

void WavePreviewWidget::setPreviewFrames(s32 frames) {
    if (mPreviewFrames != frames) {
        mPreviewFrames = frames;
        update();
    }
}

f32 WavePreviewWidget::sampleMultiplier(f32 frame) const {
    const f32 phase = frame * mFrequency / 128.0f;
    const f32 t = qCos(phase * 2.0f * std::numbers::pi_v<f32>);
    return 1.0f - (t * 0.5f + 0.5f) * mAmplitude;
}

QSize WavePreviewWidget::sizeHint() const {
    return {300, 160};
}

QSize WavePreviewWidget::minimumSizeHint() const {
    return {200, 120};
}

f32 WavePreviewWidget::frameToX(f32 frame, const QRect& rect) const {
    return static_cast<f32>(rect.left()) + (frame / static_cast<f32>(mPreviewFrames)) * static_cast<f32>(rect.width());
};

f32 WavePreviewWidget::multiplierToY(f32 mult, const QRect& rect) const {
    return static_cast<f32>(rect.top() + rect.height()) - mult * static_cast<f32>(rect.height());
};

WavePreviewWidget::GraphStyle WavePreviewWidget::computeStyle() const {
    GraphStyle style;

    style.labelFont = font();
    style.titleFont = font();

    const auto pal = palette();

    style.background = pal.color(QPalette::Base);
    style.majorGrid = pal.color(QPalette::Mid);

    style.axis = pal.color(QPalette::WindowText);
    style.axis.setAlpha(160);

    style.wave = QColor{100, 180, 255};

    return style;
}

WavePreviewWidget::GraphLayout WavePreviewWidget::computeLayout(const GraphStyle& style) const {
    GraphLayout layout;

    const QFontMetrics labelMetrics(style.labelFont);
    const QFontMetrics titleMetrics(style.titleFont);

    // Graph Rect

    constexpr s32 tickLength = 4;
    constexpr s32 labelGap = 4;
    constexpr s32 axisGap = 4;

    const s32 yLabelWidth = labelMetrics.horizontalAdvance("1.0");

    const s32 leftPadding = tickLength + labelGap + yLabelWidth + axisGap + titleMetrics.height();
    const s32 bottomPadding = tickLength + labelGap + labelMetrics.height() + axisGap + titleMetrics.height();

    layout.graphRect = {
        leftPadding,
        sPaddingTop,
        width() - leftPadding - sPaddingRight,
        height() - sPaddingTop - bottomPadding
    };

    const QRect& graph = layout.graphRect;

    // X Ticks

    for (s32 i = 0; i < sTickCountX; ++i) {
        auto& tick = layout.xTicks[i];

        auto tickValue = static_cast<f32>(i) / static_cast<f32>(sTickCountX - 1);

        const f32 frame = tickValue * static_cast<f32>(mPreviewFrames);

        tick.label = QString::number(static_cast<s32>(std::round(frame)));

        const QSize textSize = labelMetrics.size(Qt::TextSingleLine, tick.label);

        const s32 x = (i == sTickCountX - 1) ? graph.right() : static_cast<s32>(frameToX(frame, layout.graphRect));

        tick.tickLine = {
            x,
            graph.bottom(),
            x,
            graph.bottom() + tickLength
        };

        tick.gridLine = {
            x,
            graph.top(),
            x,
            graph.bottom()
        };

        tick.labelRect = {
            x - textSize.width() / 2,
            graph.bottom() + tickLength + labelGap,
            textSize.width(),
            textSize.height()
        };
    }

    // Y Ticks

    for (s32 i = 0; i < sTickCountY; ++i) {
        auto& tick = layout.yTicks[i];

        auto tickValue = static_cast<f32>(i) / static_cast<f32>(sTickCountY - 1);

        tick.label = QString::number(tickValue, 'f', 1);

        const QSize textSize = labelMetrics.size(Qt::TextSingleLine, tick.label);

        const s32 y = static_cast<s32>(multiplierToY(tickValue, layout.graphRect));

        tick.tickLine = {
            graph.left() - tickLength,
            y,
            graph.left(),
            y
        };

        tick.gridLine = {
            graph.left(),
            y,
            graph.right(),
            y
        };

        tick.labelRect = {
            graph.left() - tickLength - labelGap - textSize.width(),
            y - textSize.height() / 2,
            textSize.width(),
            textSize.height()
        };
    }

    // X Title

    {
        const QString title = "Frames";
        const QSize size = titleMetrics.size(Qt::TextSingleLine, title);

        layout.xTitleRect = {
            graph.center().x() - size.width() / 2,
            graph.bottom() + bottomPadding - size.height(),
            size.width(),
            size.height()
        };
    }

    // Y Title
    layout.yTitle = {
        "Multiplier",
        QPointF(
            titleMetrics.ascent() / 2.0,
            graph.center().y()
        ),
        -90.0f
    };

    // Wave path
    layout.wavePath = QPainterPath{};
    {
        const f32 rWidth = static_cast<f32>(graph.width());
        bool first = true;
        for (s32 px = 0; px <= graph.width(); ++px) {
            const f32 frame = static_cast<f32>(px) / rWidth * static_cast<f32>(mPreviewFrames);
            const f32 mult = sampleMultiplier(frame);
            const QPointF pt{
                static_cast<f32>(graph.x() + px),
                multiplierToY(mult, layout.graphRect)
            };
            if (first) {
                layout.wavePath.moveTo(pt);
                first = false;
            } else {
                layout.wavePath.lineTo(pt);
            }
        }
    }

    return layout;
}

void WavePreviewWidget::drawBackground(QPainter& painter, const GraphLayout& layout, const GraphStyle& colors) const {
    painter.save();

    painter.setPen(Qt::NoPen);
    painter.setBrush(colors.background);
    painter.drawRect(layout.graphRect);

    painter.restore();
}

void WavePreviewWidget::drawGrid(QPainter& painter, const GraphLayout& layout, const GraphStyle& style) const {
    painter.save();

    painter.setPen(style.majorGrid);

    for (const auto& tick : layout.xTicks) {
        painter.drawLine(tick.gridLine);
    }

    for (const auto& tick : layout.yTicks) {
        painter.drawLine(tick.gridLine);
    }

    painter.restore();
}

void WavePreviewWidget::drawWave(QPainter& painter, const GraphLayout& layout, const GraphStyle& style) const {
    painter.save();

    QPen wavePen(style.wave, 2);
    wavePen.setCapStyle(Qt::RoundCap);
    wavePen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(wavePen);
    painter.drawPath(layout.wavePath);

    painter.restore();
}

void WavePreviewWidget::drawXAxis(QPainter& painter, const GraphLayout& layout, const GraphStyle& style) const {
    painter.save();

    painter.setFont(style.labelFont);
    painter.setPen(style.axis);

    for (const auto& tick : layout.xTicks) {
        painter.drawLine(tick.tickLine);
        painter.drawText(tick.labelRect, Qt::AlignCenter, tick.label);
    }

    painter.setFont(style.titleFont);
    painter.drawText(layout.xTitleRect, Qt::AlignCenter, "Frames");

    painter.restore();
}

void WavePreviewWidget::drawYAxis(QPainter& painter, const GraphLayout& layout, const GraphStyle& style) const  {
    painter.save();

    painter.setFont(style.labelFont);
    painter.setPen(style.axis);

    for (const auto& tick : layout.yTicks) {
        painter.drawLine(tick.tickLine);
        painter.drawText(tick.labelRect, Qt::AlignRight | Qt::AlignVCenter, tick.label);
    }

    painter.setFont(style.titleFont);
    painter.setPen(style.axis);

    PaintUtil::drawRotatedText(painter, layout.yTitle, Qt::AlignCenter);

    painter.restore();
}

void WavePreviewWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    auto style = computeStyle();
    auto layout = computeLayout(style);

    if (!layout.graphRect.isValid()) {
        return;
    }

    drawBackground(painter, layout, style);
    drawGrid(painter, layout, style);
    drawXAxis(painter, layout, style);
    drawYAxis(painter, layout, style);
    drawWave(painter, layout, style);
}


// ==========================================================================//
