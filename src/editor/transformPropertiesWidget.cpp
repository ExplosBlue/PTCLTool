#include "editor/transformPropertiesWidget.h"

#include "math/util.h"

#include <QFormLayout>

TransformPropertiesWidget::TransformPropertiesWidget(QWidget* parent) :
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

void TransformPropertiesWidget::setProperties(const Ptcl::TransformProperties& properties) {
    QSignalBlocker b1(mTranslationSpinBox);
    QSignalBlocker b2(mRotationSpinBox);
    QSignalBlocker b3(mScaleSpinBox);

    mProps = properties;

    // TODO: VectorSpinBox needs to be reworked to accept custom vector type

    auto trans = Math::Util::getTranslation(mProps.transformRT);
    QVector3D translation{trans.getX(), trans.getY(), trans.getZ() };
    mTranslationSpinBox.setVector(translation);

    auto rot = Math::Util::getRotationEuler(mProps.transformRT);

    rot.setX(Math::Util::to180(rot.getX()));
    rot.setY(Math::Util::to180(rot.getY()));
    rot.setZ(Math::Util::to180(rot.getZ()));

    QVector3D rotation{rot.getX(), rot.getY(), rot.getZ() };
    mRotationSpinBox.setVector(rotation);

    auto sc = Math::Util::getScale(mProps.transformSRT);
    QVector3D scale{sc.getX(), sc.getY(), sc.getZ() };
    mScaleSpinBox.setVector(scale);
}

void TransformPropertiesWidget::rebuildMatrices() {

    // TODO: VectorSpinBox needs to be reworked to accept custom vector type

    const auto rt = mRotationSpinBox.getVector();
    const Math::Vector3f rotation = {rt.x(), rt.y(), rt.z() };

    const auto trans = mTranslationSpinBox.getVector();
    const Math::Vector3f translation = {trans.x(), trans.y(), trans.z() };

    const auto mtxR = Math::Util::eulerToRotationMatrix(rotation);

    Math::Matrix34f mtxRT;
    for (s32 r = 0; r < 3; ++r) {
        for (s32 c = 0; c < 3; ++c) {
            mtxRT(r, c) = mtxR(r, c);
        }
        mtxRT(r, 3) = translation[r];
    }

    mProps.transformRT = mtxRT;

    const auto sc = mScaleSpinBox.getVector();
    const Math::Vector3f scale = { sc.x(), sc.y(), sc.z() };

    Math::Matrix34f mtxSRT;
    for (s32 r = 0; r < 3; ++r) {
        mtxSRT(r, 0) = mtxRT(r, 0) * scale.getX();
        mtxSRT(r, 1) = mtxRT(r, 1) * scale.getY();
        mtxSRT(r, 2) = mtxRT(r, 2) * scale.getZ();
        mtxSRT(r, 3) = mtxRT(r, 3);
    }

    mProps.transformSRT = mtxSRT;
}
