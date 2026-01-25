#pragma once

#include "editor/vectorSpinBox.h"
#include "ptcl/ptclEmitter.h"



// ========================================================================== //


class TransformPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit TransformPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::TransformProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::Emitter::TransformProperties& properties);

private:
    void rebuildMatrices();

private:
    Ptcl::Emitter::TransformProperties mProps{};

    VectorSpinBox<Math::Vector3f> mScaleSpinBox;
    VectorSpinBox<Math::Vector3f> mRotationSpinBox;
    VectorSpinBox<Math::Vector3f> mTranslationSpinBox;
};


// ========================================================================== //
