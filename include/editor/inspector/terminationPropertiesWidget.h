#pragma once

#include "editor/inspector/inspectorWidgetBase.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class TerminationPropertiesWidget final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit TerminationPropertiesWidget(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QCheckBox mIsStopEmitCheckBox;
    QDoubleSpinBox mAlphaAddInSpinBox;
};


// ========================================================================== //


} // namespace PtclEditor
