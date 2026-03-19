#pragma once

#include "editor/components/rgbaColorWidget.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildColorInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ChildColorInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    RGBAColorWidget mColor0Widget{};
    RGBAColorWidget mColor1Widget{};
    QCheckBox mInheritColorCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
