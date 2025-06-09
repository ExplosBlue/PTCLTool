#include "editor/clickableLabel.h"

#include <QMouseEvent>


// ========================================================================== //


ClickableLabel::ClickableLabel(QWidget* parent) :
    QLabel(parent) {
    setCursor(Qt::PointingHandCursor);
}

void ClickableLabel::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    } else if (event->button() == Qt::RightButton) {
        emit rightClicked();
    }

    QLabel::mouseMoveEvent(event);
}


// ========================================================================== //
