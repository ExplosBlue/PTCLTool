#include "editor/inspector/transformInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


TransformInspector::TransformInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Translation", &mTranslationSpinBox);
    mainLayout->addRow("Rotation", &mRotationSpinBox);
    mainLayout->addRow("Scale", &mScaleSpinBox);

    setupConnections();
}

void TransformInspector::setupConnections() {
    connect(&mTranslationSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto translation = mTranslationSpinBox.getVector();
        setEmitterProperty(
            "Set Translation",
            "SetTranslation",
            &Ptcl::Emitter::translation,
            &Ptcl::Emitter::setTranslation,
            translation
        );
    });

    connect(&mRotationSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto rotation = mRotationSpinBox.getVector();
        setEmitterProperty(
            "Set Rotation",
            "SetRotation",
            &Ptcl::Emitter::rotation,
            &Ptcl::Emitter::setRotation,
            rotation
        );
    });

    connect(&mScaleSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto scale = mScaleSpinBox.getVector();
        setEmitterProperty(
            "Set Scale",
            "SetScale",
            &Ptcl::Emitter::scale,
            &Ptcl::Emitter::setScale,
            scale
        );
    });
}

void TransformInspector::populateProperties() {
    QSignalBlocker b1(mTranslationSpinBox);
    QSignalBlocker b2(mRotationSpinBox);
    QSignalBlocker b3(mScaleSpinBox);

    mTranslationSpinBox.setVector(mEmitter->translation());
    mRotationSpinBox.setVector(mEmitter->rotation());
    mScaleSpinBox.setVector(mEmitter->scale());
}


// ========================================================================== //


} // namespace PtclEditor
