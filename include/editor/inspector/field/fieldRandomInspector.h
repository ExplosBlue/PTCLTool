#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldRandomInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit FieldRandomInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QWidget* mControlsWidget{nullptr};
    QSpinBox mRandomBlankSpinBox{};
    VectorSpinBox<Math::Vector3f> mRandomVelAddSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
