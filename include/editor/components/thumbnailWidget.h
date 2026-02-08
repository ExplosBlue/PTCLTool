#pragma once

#include <QWidget>


// ========================================================================== //


class ThumbnailWidget : public QWidget {
    Q_OBJECT
public:
    explicit ThumbnailWidget(QWidget* parent = nullptr);

    void setPixmap(const QPixmap& pixmap);
    void setThumbnailSize(QSize size);

    void clear();

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    QPixmap mPixmap{};
    QSize mThumbnailSize{64, 64};
};


// ========================================================================== //
