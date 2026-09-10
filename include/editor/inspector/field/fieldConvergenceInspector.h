#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QWidget>


namespace PtclTool {


// ========================================================================== //


class FieldConvergenceInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit FieldConvergenceInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();
    void setWidgetsEnabled(bool enable);

private:
    VectorSpinBox<Math::Vector3f> mPosSpinBox{};
    QDoubleSpinBox mSpeedSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclTool
