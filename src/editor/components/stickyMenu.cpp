#include "editor/components/stickyMenu.h"

#include <QMouseEvent>


namespace PtclEditor {


// ========================================================================== //


StickyMenu::StickyMenu(QWidget* parent) :
    QMenu{parent} {}

void StickyMenu::mouseReleaseEvent(QMouseEvent* event) {
    QAction* action = actionAt(event->pos());
    if (action) {
        action->trigger();
        return;
    }

    QMenu::mouseReleaseEvent(event);
}


// ========================================================================== //


} // namespace PtclEditor
