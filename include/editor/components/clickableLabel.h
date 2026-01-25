#pragma once

#include <QLabel>


// ========================================================================== //


class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget* parent = nullptr);

signals:
    void clicked();
    void rightClicked();

protected:
    void mousePressEvent(QMouseEvent* event) override;
};


// ========================================================================== //
