#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/emitterWidget/emitterWidgetBase.h"

#include <QComboBox>
#include <QLabel>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class RotationPropertiesWidget final : public EmitterWidgetBase {
    Q_OBJECT
public:
    explicit RotationPropertiesWidget(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();
    void updateAxis();

private:
    EnumComboBox<Ptcl::RotType> mRotTypeSpinBox{};
    VectorSpinBox<Math::Vector3f> mInitRotSpinBox{};
    VectorSpinBox<Math::Vector3f> mInitRotRandSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotVelSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotVelRandSpinBox{};
    VectorSpinBox<Math::Vector2f> mRotBasisSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
