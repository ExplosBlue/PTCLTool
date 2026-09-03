#include "editor/emitterList/emitterFilterMenu.h"


namespace PtclTool {


// ========================================================================== //


EmitterFilterMenu::EmitterFilterMenu(QWidget* parent) :
    QMenu{parent} {

    auto* simpleAction  = addAction("Simple Emitters");
    auto* complexAction = addAction("Complex Emitters");
    auto* compactAction = addAction("Compact Emitters");

    addSeparator();

    auto* allAction = addAction("Show All");

    for (auto* act : { simpleAction, complexAction, compactAction }) {
        act->setCheckable(true);
        act->setChecked(true);
    }

    connect(allAction, &QAction::triggered, this, [simpleAction, complexAction, compactAction, this] {
        simpleAction->setChecked(true);
        complexAction->setChecked(true);
        compactAction->setChecked(true);
        emit emitterFilterChanged({
            EmitterFilterFlag::Simple,
            EmitterFilterFlag::Complex,
            EmitterFilterFlag::Compact
        });
    });

    auto updateFilter = [simpleAction, complexAction, compactAction, this] {
        EmitterFilter filter{};
        if (simpleAction->isChecked())  { filter.enable(EmitterFilterFlag::Simple); }
        if (complexAction->isChecked()) { filter.enable(EmitterFilterFlag::Complex); }
        if (compactAction->isChecked()) { filter.enable(EmitterFilterFlag::Compact); }
        emit emitterFilterChanged(filter);
    };

    connect(simpleAction,  &QAction::toggled, this, updateFilter);
    connect(complexAction, &QAction::toggled, this, updateFilter);
    connect(compactAction, &QAction::toggled, this, updateFilter);

}


// ========================================================================== //


} // namespace PtclTool
