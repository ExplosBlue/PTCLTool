#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"


namespace PtclEditor {


// ========================================================================== //


class TransformPropertiesWidget final : public InspectorWidgetBase {
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
