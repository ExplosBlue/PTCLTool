#include "editor/loadingSpinner.h"

#include <QPainter>


// ========================================================================== //


LoadingSpinner::LoadingSpinner(QObject* parent) :
    QObject{parent}, mCanvasSize{128, 128}, mRotationSpeed{180} {
    connect(&mTimer, &QTimer::timeout, this, [this]() {
        qint64 ms = mElapsed.elapsed();
        qreal angle = fmod((static_cast<double>(ms) / 1000.0) * mRotationSpeed, 360.0);
        emit frameUpdated(renderFrame(angle));
    });

    mTimer.setInterval(16);
}


void LoadingSpinner::start() {
    mElapsed.restart();
    mTimer.start();
}

void LoadingSpinner::stop() {
    mTimer.stop();
}

void LoadingSpinner::setCanvasSize(const QSize& size) {
    mCanvasSize = size;
}

QPixmap LoadingSpinner::renderFrame(qreal angle) {
    QPixmap canvas(mCanvasSize);
    canvas.fill(Qt::transparent);

    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int w = mCanvasSize.width();
    int h = mCanvasSize.height();
    int size = qMin(w, h);
    int thickness = size / 10;
    int radius = size / 2 - thickness;

    painter.translate(static_cast<qreal>(w) / 2.0, static_cast<qreal>(h) / 2.0);
    painter.rotate(angle);

    QRectF arcRect(-radius, -radius, radius * 2, radius * 2);
    QPen pen(Qt::gray, thickness, Qt::SolidLine, Qt::RoundCap);

    painter.setPen(pen);
    painter.drawArc(arcRect, 0 * 16, 120 * 16);

    return canvas;
}


// ========================================================================== //
