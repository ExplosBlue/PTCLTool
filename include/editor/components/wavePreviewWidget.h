#pragma once

#include "typedefs.h"
#include "util/paintUtil.h"

#include <QPainterPath>
#include <QWidget>

#include <array>


// ==========================================================================//


class WavePreviewWidget final : public QWidget {
    Q_OBJECT
private:
    static constexpr s32 sTickCountX = 8;
    static constexpr s32 sTickCountY = 5;
    static constexpr s32 sPaddingTop = 6;
    static constexpr s32 sPaddingRight = 16;
    static constexpr s32 sPaddingBottom = 36;

private:
    struct Tick {
        QString label;

        QLine tickLine;
        QLine gridLine;

        QRect labelRect;
    };

    struct GraphLayout {
        QRect graphRect;

        QRect xTitleRect;
        RotatedText yTitle;

        std::array<Tick, sTickCountX> xTicks;
        std::array<Tick, sTickCountY> yTicks;

        QPainterPath wavePath;
    };

    struct GraphStyle {
        QFont labelFont;
        QFont titleFont;

        QColor background;
        QColor majorGrid;
        QColor axis;
        QColor wave;
    };

public:
    explicit WavePreviewWidget(QWidget* parent = nullptr);

    void setAmplitude(f32 amplitude);
    void setFrequency(f32 frequency);
    void setPreviewFrames(s32 frames);

protected:
    void paintEvent(QPaintEvent* event) final;
    QSize sizeHint() const final;
    QSize minimumSizeHint() const final;

private:
    f32 sampleMultiplier(f32 frame) const;

    f32 frameToX(f32 frame, const QRect& rect) const;
    f32 multiplierToY(f32 mult, const QRect& rect) const;

    GraphStyle computeStyle() const;
    GraphLayout computeLayout(const GraphStyle& style) const;

    void drawBackground(QPainter& painter, const GraphLayout& layout, const GraphStyle& style) const;
    void drawGrid(QPainter& painter, const GraphLayout& layout, const GraphStyle& style) const;
    void drawWave(QPainter& painter, const GraphLayout& layout, const GraphStyle& style) const;
    void drawXAxis(QPainter& painter, const GraphLayout& layout, const GraphStyle& style) const;
    void drawYAxis(QPainter& painter, const GraphLayout& layout, const GraphStyle& style) const;

private:
    f32 mAmplitude{};
    f32 mFrequency{};
    s32 mPreviewFrames{};
};


// ==========================================================================//
