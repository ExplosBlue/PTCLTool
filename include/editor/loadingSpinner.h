#ifndef LOADINGSPINNER_H
#define LOADINGSPINNER_H

#include <QObject>
#include <QTimer>
#include <QElapsedTimer>
#include <QPixmap>

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

#endif // LOADINGSPINNER_H
