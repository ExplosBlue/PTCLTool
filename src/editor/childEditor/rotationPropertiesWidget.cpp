#include "editor/childEditor/rotationPropertiesWidget.h"

#include "math/util.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::RotationPropertiesWidget::RotationPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    mInheritRotCheckBox.setText("Inherit Parent Rotation");

    mainLayout->addRow("Rotation:", &mInheritRotCheckBox);
    mainLayout->addRow("Rotation Type:", &mRotTypeSpinBox);
    mainLayout->addRow("Initial Rotation:", &mInitRotSpinBox);
    mainLayout->addRow("Initial Rotation Random:", &mInitRotRandSpinBox);
    mainLayout->addRow("Rotation Speed:", &mRotVelSpinBox);
    mainLayout->addRow("Rotation Speed Random:", &mRotVelRandSpinBox);
    mainLayout->addRow("Rotation Pivot Offset:", &mRotBasisSpinBox);

    setupConnections();
}

void ChildEditorWidget::RotationPropertiesWidget::setupConnections() {
    auto deg2idxVec = [](const Math::Vector3f& v) {
        Math::Vector3i result {
            Math::Util::deg2idx(v.getX()),
            Math::Util::deg2idx(v.getY()),
            Math::Util::deg2idx(v.getZ())
        };
        return result;
    };

    // Inherit Rotation
    connect(&mInheritRotCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        emit inheritRotationUpdated(checked);
    });

    // RotType
    connect(&mRotTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.rotType = mRotTypeSpinBox.currentEnum();
        updateAxis();
        emit propertiesUpdated(mProps);
    });

    // Initial Rotation
    connect(&mInitRotSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        mProps.initRot = deg2idxVec(mInitRotSpinBox.getVector());
        emit propertiesUpdated(mProps);
    });

    // Initial Rotation Rand
    connect(&mInitRotRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        mProps.initRotRand = deg2idxVec(mInitRotRandSpinBox.getVector());
        emit propertiesUpdated(mProps);
    });

    // Rotation Speed
    connect(&mRotVelSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        mProps.rotVel = deg2idxVec(mRotVelSpinBox.getVector());
        emit propertiesUpdated(mProps);
    });

    // Rotation Speed Rand
    connect(&mRotVelRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        mProps.rotVelRand = deg2idxVec(mRotVelRandSpinBox.getVector());
        emit propertiesUpdated(mProps);
    });

    // Rotation Pivot
    connect(&mRotBasisSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mProps.rotBasis = mRotBasisSpinBox.getVector();
        emit propertiesUpdated(mProps);
    });
}


void ChildEditorWidget::RotationPropertiesWidget::setProperties(const Ptcl::ChildData::RotationProperties& properties, bool inheritRotation) {
    QSignalBlocker b1(mRotTypeSpinBox);
    QSignalBlocker b2(mInitRotSpinBox);
    QSignalBlocker b3(mInitRotRandSpinBox);
    QSignalBlocker b4(mRotVelSpinBox);
    QSignalBlocker b5(mRotVelRandSpinBox);
    QSignalBlocker b6(mRotBasisSpinBox);
    QSignalBlocker b7(mInheritRotCheckBox);

    mProps = properties;

    auto idx2degVec = [](const Math::Vector3i& v) {
        return Math::Vector3f {
            Math::Util::to180(Math::Util::idx2deg(v.getX())),
            Math::Util::to180(Math::Util::idx2deg(v.getY())),
            Math::Util::to180(Math::Util::idx2deg(v.getZ()))
        };
    };

    mRotTypeSpinBox.setCurrentEnum(mProps.rotType);
    mInitRotSpinBox.setVector(idx2degVec(mProps.initRot));
    mInitRotRandSpinBox.setVector(idx2degVec(mProps.initRotRand));
    mRotVelSpinBox.setVector(idx2degVec(mProps.rotVel));
    mRotVelRandSpinBox.setVector(idx2degVec(mProps.rotVelRand));
    mRotBasisSpinBox.setVector(mProps.rotBasis);
    mInheritRotCheckBox.setChecked(inheritRotation);

    updateAxis();
}

void ChildEditorWidget::RotationPropertiesWidget::updateAxis() {
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


} // namespace PtclEditor
