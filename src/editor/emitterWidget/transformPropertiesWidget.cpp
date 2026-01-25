#include "editor/emitterWidget/transformPropertiesWidget.h"

#include "math/util.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


EmitterWidget::TransformPropertiesWidget::TransformPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Translation", &mTranslationSpinBox);
    mainLayout->addRow("Rotation", &mRotationSpinBox);
    mainLayout->addRow("Scale", &mScaleSpinBox);

    connect(&mTranslationSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        rebuildMatrices();
        emit propertiesUpdated(mProps);
    });

    connect(&mRotationSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        rebuildMatrices();
        emit propertiesUpdated(mProps);
    });

    connect(&mScaleSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        rebuildMatrices();
        emit propertiesUpdated(mProps);
    });
}

void EmitterWidget::TransformPropertiesWidget::setProperties(const Ptcl::Emitter::TransformProperties& properties) {
    QSignalBlocker b1(mTranslationSpinBox);
    QSignalBlocker b2(mRotationSpinBox);
    QSignalBlocker b3(mScaleSpinBox);

    mProps = properties;

    auto translation = Math::Util::getTranslation(mProps.transformRT);
    mTranslationSpinBox.setVector(translation);

    auto rotation = Math::Util::getRotationEuler(mProps.transformRT);

    rotation.setX(Math::Util::to180(rotation.getX()));
    rotation.setY(Math::Util::to180(rotation.getY()));
    rotation.setZ(Math::Util::to180(rotation.getZ()));

    mRotationSpinBox.setVector(rotation);

    auto scale = Math::Util::getScale(mProps.transformSRT);
    mScaleSpinBox.setVector(scale);
}

void EmitterWidget::TransformPropertiesWidget::rebuildMatrices() {
    const auto rotation = mRotationSpinBox.getVector();
    const auto translation = mTranslationSpinBox.getVector();

    const auto mtxR = Math::Util::eulerToRotationMatrix(rotation);

    Math::Matrix34f mtxRT;
    for (s32 r = 0; r < 3; ++r) {
        for (s32 c = 0; c < 3; ++c) {
            mtxRT(r, c) = mtxR(r, c);
        }
        mtxRT(r, 3) = translation[r];
    }

    mProps.transformRT = mtxRT;

    const auto scale = mScaleSpinBox.getVector();

    Math::Matrix34f mtxSRT;
    for (s32 r = 0; r < 3; ++r) {
        mtxSRT(r, 0) = mtxRT(r, 0) * scale.getX();
        mtxSRT(r, 1) = mtxRT(r, 1) * scale.getY();
        mtxSRT(r, 2) = mtxRT(r, 2) * scale.getZ();
        mtxSRT(r, 3) = mtxRT(r, 3);
    }

    mProps.transformSRT = mtxSRT;
}


// ========================================================================== //


} // namespace PtclEditor
