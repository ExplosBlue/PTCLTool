#pragma once

#include <QElapsedTimer>
#include <QObject>
#include <QPixmap>
#include <QTimer>


// ========================================================================== //


class LoadingSpinner : public QObject {
    Q_OBJECT
public:
    explicit LoadingSpinner(QObject* parent = nullptr);

    void start();
    void stop();

    void setCanvasSize(const QSize& size);

signals:
    void frameUpdated(const QPixmap& pixmap);

private:
    QPixmap renderFrame(qreal angle);

private:
    QSize mCanvasSize;
    QTimer mTimer;
    QElapsedTimer mElapsed;
    int mRotationSpeed;
};


// ========================================================================== //
