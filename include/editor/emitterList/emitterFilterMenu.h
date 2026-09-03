#pragma once

#include "editor/emitterList/emitterListTypes.h"

#include <QMenu>


namespace PtclTool {


// ========================================================================== //

class EmitterFilterMenu : public QMenu {
    Q_OBJECT
public:
    explicit EmitterFilterMenu(QWidget* parent = nullptr);

signals:
    void emitterFilterChanged(const PtclTool::EmitterFilter& filter);
};


// ========================================================================== //


} // namespace PtclTool
