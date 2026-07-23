#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildRotationScaleInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ChildRotationScaleInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();
    void updateAxis();

private:
    // Rotation widgets
    EnumComboBox<Ptcl::RotType> mRotTypeComboBox{};
    VectorSpinBox<Math::Vector3f> mInitRotSpinBox{};
    VectorSpinBox<Math::Vector3f> mInitRotRandSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotVelSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotVelRandSpinBox{};
    VectorSpinBox<Math::Vector2f> mRotBasisSpinBox{};
    QCheckBox mInheritRotCheckBox{};

    // Scale widgets
    VectorSpinBox<Math::Vector2f> mScaleSpinBox{};
    VectorSpinBox<Math::Vector2f> mScaleTargetSpinBox{};
    QDoubleSpinBox mInheritRateSpinBox{};
    QSpinBox mStartFrameSpinBox{};
    QCheckBox mInheritScaleCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
