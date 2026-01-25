#include "editor/rotationPropertiesWidget.h"
#include "math/util.h"

#include <QFormLayout>

// ========================================================================== //


RotationPropertiesWidget::RotationPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    // RotType
    mainLayout->addRow("Rotation Type", &mRotTypeSpinBox);
    connect(&mRotTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.rotType = mRotTypeSpinBox.currentEnum();
        updateAxis();
        emit propertiesUpdated(mProps);
    });

    auto deg2idxVec = [](const Math::Vector3f& v) {
        Math::Vector3i result {
            Math::Util::deg2idx(v.getX()),
            Math::Util::deg2idx(v.getY()),
            Math::Util::deg2idx(v.getZ())
        };
        return result;
    };

    // Initial Rotation
    mainLayout->addRow("Initial Rotation", &mInitRotSpinBox);
    connect(&mInitRotSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        mProps.initRot = deg2idxVec(mInitRotSpinBox.getVector());
        emit propertiesUpdated(mProps);
    });

    // Initial Rotation Rand
    mainLayout->addRow("Initial Rotation Random", &mInitRotRandSpinBox);
    connect(&mInitRotRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        mProps.initRotRand = deg2idxVec(mInitRotRandSpinBox.getVector());
        emit propertiesUpdated(mProps);
    });

    // Rotation Speed
    mainLayout->addRow("Rotation Speed", &mRotVelSpinBox);
    connect(&mRotVelSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        mProps.rotVel = deg2idxVec(mRotVelSpinBox.getVector());
        emit propertiesUpdated(mProps);
    });

    // Rotation Speed Rand
    mainLayout->addRow("Rotation Speed Random", &mRotVelRandSpinBox);
    connect(&mRotVelRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        mProps.rotVelRand = deg2idxVec(mRotVelRandSpinBox.getVector());
        emit propertiesUpdated(mProps);
    });

    // Rotation Pivot
    mainLayout->addRow("Rotation Pivot Offset", &mRotBasisSpinBox);
    connect(&mRotBasisSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mProps.rotBasis = mRotBasisSpinBox.getVector();
        emit propertiesUpdated(mProps);
    });

    setLayout(mainLayout);

}

void RotationPropertiesWidget::setProperties(const Ptcl::Emitter::RotationProperties& properties) {
    mProps = properties;
    populateWidgets();
}

void RotationPropertiesWidget::populateWidgets() {
    auto idx2degVec = [](const Math::Vector3i& v) {
        return Math::Vector3f {
            Math::Util::to180(Math::Util::idx2deg(v.getX())),
            Math::Util::to180(Math::Util::idx2deg(v.getY())),
            Math::Util::to180(Math::Util::idx2deg(v.getZ()))
        };
    };

    QSignalBlocker b1(mRotTypeSpinBox);
    mRotTypeSpinBox.setCurrentEnum(mProps.rotType);

    QSignalBlocker b2(mInitRotSpinBox);
    mInitRotSpinBox.setVector(idx2degVec(mProps.initRot));

    QSignalBlocker b3(mInitRotRandSpinBox);
    mInitRotRandSpinBox.setVector(idx2degVec(mProps.initRotRand));

    QSignalBlocker b4(mRotVelSpinBox);
    mRotVelSpinBox.setVector(idx2degVec(mProps.rotVel));

    QSignalBlocker b5(mRotVelRandSpinBox);
    mRotVelRandSpinBox.setVector(idx2degVec(mProps.rotVelRand));

    QSignalBlocker b6(mRotBasisSpinBox);
    mRotBasisSpinBox.setVector(mProps.rotBasis);

    updateAxis();
}

void RotationPropertiesWidget::updateAxis() {
    using Axis = VectorSpinBoxBase::Axis;

    switch (mProps.rotType) {
    case Ptcl::RotType::None:
        mInitRotSpinBox.setEnabledAxis(Axis::None);
        mInitRotRandSpinBox.setEnabledAxis(Axis::None);
        mRotVelSpinBox.setEnabledAxis(Axis::None);
        mRotVelRandSpinBox.setEnabledAxis(Axis::None);
        break;
    case Ptcl::RotType::RotX:
        mInitRotSpinBox.setEnabledAxis(Axis::X);
        mInitRotRandSpinBox.setEnabledAxis(Axis::X);
        mRotVelSpinBox.setEnabledAxis(Axis::X);
        mRotVelRandSpinBox.setEnabledAxis(Axis::X);
        break;
    case Ptcl::RotType::RotY:
        mInitRotSpinBox.setEnabledAxis(Axis::Y);
        mInitRotRandSpinBox.setEnabledAxis(Axis::Y);
        mRotVelSpinBox.setEnabledAxis(Axis::Y);
        mRotVelRandSpinBox.setEnabledAxis(Axis::Y);
        break;
    case Ptcl::RotType::RotZ:
        mInitRotSpinBox.setEnabledAxis(Axis::Z);
        mInitRotRandSpinBox.setEnabledAxis(Axis::Z);
        mRotVelSpinBox.setEnabledAxis(Axis::Z);
        mRotVelRandSpinBox.setEnabledAxis(Axis::Z);
        break;
    case Ptcl::RotType::RotXYZ:
        mInitRotSpinBox.setEnabledAxis(Axis::XYZ);
        mInitRotRandSpinBox.setEnabledAxis(Axis::XYZ);
        mRotVelSpinBox.setEnabledAxis(Axis::XYZ);
        mRotVelRandSpinBox.setEnabledAxis(Axis::XYZ);
        break;
    default:
        break;
    }
}


// ========================================================================== //
