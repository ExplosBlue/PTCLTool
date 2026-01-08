#include "editor/animGraph.h"

#include <QPainter>


// ========================================================================== //


AnimGraph::AnimGraph(QWidget* parent) :
    QWidget{parent} {
    setMinimumSize(200, 150);
}

void AnimGraph::setAnim(const Ptcl::ScaleAnim& scaleAnim) {
    mAnim = scaleAnim;
    update();
}

void AnimGraph::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const int w = width();
    const int h = height();
    const int halfH = h / 2;

    // Helper lambda to map X (0-100%) to pixel
    auto mapX = [w](f32 percent) {
        return static_cast<int>((percent / 100.0f) * w);
    };

    // Helper lambda to map Y (-100 to 100) to pixel
    auto mapY = [](int graphHeight, f32 scale) {
        return static_cast<int>(((-scale + 100.0f) / 200.0f) * graphHeight);
    };

    const QVector2D s1 = mAnim.initScale;
    const QVector2D s2 = s1 + mAnim.diffScale21;
    const QVector2D s3 = s2;
    const QVector2D s4 = s3 + mAnim.diffScale32;

    auto drawGraph = [&](int yOffset, const QColor& color, const auto& get) {
        painter.save();
        painter.translate(0, yOffset);

        painter.setPen(QPen(Qt::black, 1));
        painter.drawLine(mapX(0), mapY(halfH, 0), mapX(100), mapY(halfH, 0));
        painter.drawLine(mapX(0), 0, mapX(0), halfH);

        const float v1 = get(s1);
        const float v2 = get(s2);
        const float v3 = get(s3);
        const float v4 = get(s4);

        const QPoint p1(mapX(0.0f), mapY(halfH, v1));
        const QPoint p2(mapX(mAnim.scaleSection1), mapY(halfH, v2));
        const QPoint p3(mapX(mAnim.scaleSection2), mapY(halfH, v3));
        const QPoint p4(mapX(100.0f), mapY(halfH, v4));

        painter.setPen(QPen(color, 2));
        painter.drawPolyline(QPolygon({ p1, p2, p3, p4 }));

        painter.setBrush(Qt::red);
        for (const QPoint& pt : { p1, p2, p3, p4 }) {
            painter.drawEllipse(pt, 4, 4);
        }

        painter.restore();
    };

    // Draw X axis graph
    drawGraph(0, Qt::blue, [](const QVector2D& v) { return v.x(); });

    // Draw Y axis graph
    drawGraph(halfH, Qt::green, [](const QVector2D& v) { return v.y(); });
}

// ========================================================================== //
