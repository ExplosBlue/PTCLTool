#pragma once

#include <QMenu>


namespace PtclEditor {


// ========================================================================== //


class StickyMenu : public QMenu {
    Q_OBJECT
public:
    explicit StickyMenu(QWidget* parent = nullptr);

protected:
    void mouseReleaseEvent(QMouseEvent* event) override;
};


// ========================================================================== //


} // namespace PtclEditor
