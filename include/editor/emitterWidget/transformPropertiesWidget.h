#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/emitterWidget/emitterWidgetBase.h"


namespace PtclEditor {


// ========================================================================== //


class TransformPropertiesWidget final : public EmitterWidgetBase {
    Q_OBJECT
public:
    explicit TransformPropertiesWidget(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    VectorSpinBox<Math::Vector3f> mScaleSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotationSpinBox{};
    VectorSpinBox<Math::Vector3f> mTranslationSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
