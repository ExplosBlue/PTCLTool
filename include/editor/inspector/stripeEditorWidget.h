#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class StripeEditorWidget final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit StripeEditorWidget(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QComboBox mTypeComboBox{};
    QSpinBox mNumHistSpinBox{};
    QDoubleSpinBox mStartAlphaSpinBox{};
    QDoubleSpinBox mEndAlphaSpinBox{};
    VectorSpinBox<Math::Vector2f> mUVScrollSpeedSpinBox{};
    QSpinBox mHistStepSpinBox{};
    QDoubleSpinBox mDirIntepolateSpinBox{};
    QCheckBox mEmitterCoordCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
