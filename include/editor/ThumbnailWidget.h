#ifndef THUMBNAILWIDGET_H
#define THUMBNAILWIDGET_H

#include <QWidget>

class ThumbnailWidget : public QWidget {
    Q_OBJECT
public:
    explicit ThumbnailWidget(QWidget* parent = nullptr);

    void setPixmap(const QPixmap& pixmap);
    void setThumbnailSize(QSize size);

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

private:
    QPixmap mPixmap;
    QSize mThumbnailSize;

    void drawCheckerBackground(QPainter& painter, const QRect& rect);
};

#endif // THUMBNAILWIDGET_H
