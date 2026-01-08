#include "editor/animGraph.h"

#include <QPainter>


// ========================================================================== //


AnimGraph::AnimGraph(QWidget* parent) :
    QWidget{parent} {
    setMinimumSize(200, 200);
}

void AnimGraph::setAnim(const Ptcl::ScaleAnim& scaleAnim) {
    mAnim = scaleAnim;
    update();
}

GraphRange AnimGraph::computeGraphRange(const std::initializer_list<f32>& values) {
    f32 minV = std::numeric_limits<f32>::max();
    f32 maxV = std::numeric_limits<f32>::lowest();

    for (f32 v : values) {
        minV = std::min(minV, v);
        maxV = std::max(maxV, v);
    }

    if (minV == maxV) {
        minV -= 1.0f;
        maxV += 1.0f;
    }

    const f32 padding = (maxV - minV) * 0.1f;
    return { minV - padding, maxV + padding };
}

// void AnimGraph::drawGrid(QPainter& painter, s32 width, s32 height) {
//     painter.save();

//     // --- Vertical checker (time bands) ---
//     constexpr int xDivs = 10;
//     QColor color1(245, 245, 245);
//     QColor color2(225, 225, 225);

//     for (int x = 0; x < xDivs; ++x) {
//         // compute left and right edges as float, then round
//         const int left  = int((float(x) / xDivs) * width);
//         const int right = int((float(x + 1) / xDivs) * width);
//         const int bandW = right - left;

//         QRect rect(left, 0, bandW, height);
//         painter.fillRect(rect, (x % 2) == 0 ? color1 : color2);
//     }

//     // --- Horizontal lines (Y value divisions) ---
//     constexpr int yMajorDivs = 8;
//     painter.setPen(QPen(QColor(200, 200, 200), 1));
//     for (int i = 0; i <= yMajorDivs; ++i) {
//         const int y = int((float(i) / yMajorDivs) * height);
//         painter.drawLine(0, y, width, y);
//     }

//     // --- Center horizontal line (0 value) and Y axis ---
//     painter.setPen(QPen(QColor(120, 120, 120), 2));
//     painter.drawLine(0, height / 2, width, height / 2); // 0 line
//     painter.drawLine(0, 0, 0, height);                  // Y axis

//     painter.restore();
// }

void AnimGraph::drawGrid(QPainter& painter, s32 width, s32 height) {
    painter.save();

    constexpr s32 xMajorDivs = 10;
    constexpr s32 xMinorDivs = 3;
    constexpr s32 yMajorDivs = 4;

    // Background
    painter.fillRect(0, 0, width, height, QColor(48, 48, 48));

    // X Minor Grid
    painter.setPen(QPen(QColor(42, 42, 42), 1));
    for (int i = 0; i <= xMajorDivs * xMinorDivs; ++i) {
        const f32 t = f32(i) / f32(xMajorDivs * xMinorDivs);
        const s32 x = s32(t * width);
        painter.drawLine(x, 0, x, height);
    }

    // X Major Grid
    painter.setPen(QPen(QColor(26, 26, 26), 1));
    for (int i = 0; i <= xMajorDivs; ++i) {
        const f32 t = f32(i) / f32(xMajorDivs);
        const s32 x = s32(t * width);
        painter.drawLine(x, 0, x, height);
    }

    // Y Major Grid
    painter.setPen(QPen(QColor(26, 26, 26), 1));
    for (int i = 0; i <= yMajorDivs; ++i) {
        const f32 t = f32(i) / f32(yMajorDivs);
        const s32 y = s32(t * height);
        painter.drawLine(0, y, width, y);
    }

    // painter.setPen(QPen(QColor(120, 120, 120), 2));
    // painter.drawLine(0, height / 2, width, height / 2);
    // painter.drawLine(0, 0, 0, height);

    painter.restore();
}

void AnimGraph::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    constexpr s32 graphSpacing = 16;

    const s32 w = width();
    const s32 h = height();
    const s32 halfH = (h - graphSpacing) / 2;

    // Helper lambda to map X (0-100%) to pixel
    auto mapX = [w](f32 percent) {
        return static_cast<int>((percent / 100.0f) * w);
    };

    // Helper to generate a Y axis mapping function
    auto makeMapY = [&](int height, GraphRange range) {
        return [=](f32 value) {
            return s32(((range.max - value) / (range.max - range.min)) * height);
        };
    };

    const QVector2D s1 = mAnim.initScale;
    const QVector2D s2 = s1 + mAnim.diffScale21;
    const QVector2D s3 = s2;
    const QVector2D s4 = s3 + mAnim.diffScale32;

    auto drawGraph = [&](int yOffset, const QColor& color, const auto& get) {
        painter.save();
        painter.translate(0, yOffset);
        painter.setClipRect(0, 0, w, halfH);

        drawGrid(painter, w, halfH);

        const f32 v1 = get(s1);
        const f32 v2 = get(s2);
        const f32 v3 = get(s3);
        const f32 v4 = get(s4);

        const GraphRange range = computeGraphRange({v1, v2, v3, v4});
        const auto mapY = makeMapY(halfH, range);

        const QPoint p1(mapX(0.0f), mapY(v1));
        const QPoint p2(mapX(mAnim.scaleSection1), mapY(v2));
        const QPoint p3(mapX(mAnim.scaleSection2), mapY(v3));
        const QPoint p4(mapX(100.0f), mapY(v4));

        painter.setPen(QPen(color, 2));
        painter.drawPolyline(QPolygon({ p1, p2, p3, p4 }));

        // Selected color
        //painter.setPen(QColor(253, 133, 2));
        painter.setPen(QPen(Qt::black, 2));
        for (const QPoint& pt : { p1, p2, p3, p4 }) {
            painter.drawEllipse(pt, 4, 4);
        }

        painter.restore();
    };

    // Draw X axis graph
    drawGraph(0, QColor(238, 51, 79), [](const QVector2D& v) { return v.x(); });

    // Draw Y axis graph
    drawGraph(halfH + graphSpacing, QColor(42, 125, 212), [](const QVector2D& v) { return v.y(); });

    // Z Color: QColor(137, 214, 1)
}

// ========================================================================== //
