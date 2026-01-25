#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/emitterWidget/emitterWidget.h"

#include "ptcl/ptclEmitter.h"

#include <QComboBox>
#include <QLabel>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidget::RotationPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit RotationPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::RotationProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::Emitter::RotationProperties& properties);

private:
    void populateWidgets();
    void updateAxis();

private:
    Ptcl::Emitter::RotationProperties mProps{};

    EnumComboBox<Ptcl::RotType> mRotTypeSpinBox{};
    VectorSpinBox<Math::Vector3f> mInitRotSpinBox{};
    VectorSpinBox<Math::Vector3f> mInitRotRandSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotVelSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotVelRandSpinBox{};
    VectorSpinBox<Math::Vector2f> mRotBasisSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
