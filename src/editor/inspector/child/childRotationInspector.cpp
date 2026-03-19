#include "editor/inspector/child/childRotationInspector.h"

#include "math/util.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


ChildRotationInspector::ChildRotationInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

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

void ChildRotationInspector::setupConnections() {
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
        setEmitterProperty(
            "Toggle Child Inherit Rotation",
            "ToggleChildInheritRot",
            &Ptcl::Emitter::isChildInheritRotation,
            &Ptcl::Emitter::setChildInheritRotation,
            checked
        );
    });

    // RotType
    connect(&mRotTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mRotTypeSpinBox.currentEnum();
        setEmitterProperty(
            "Set Child Rotation Type",
            "SetChildRotationType",
            &Ptcl::Emitter::childRotationType,
            &Ptcl::Emitter::setChildRotationType,
            type
        );
    });

    // Initial Rotation
    connect(&mInitRotSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto rot = deg2idxVec(mInitRotSpinBox.getVector());
        setEmitterProperty(
            "Set Child Initial Rotation",
            "SetChildInitRotation",
            &Ptcl::Emitter::childInitialRotation,
            &Ptcl::Emitter::setChildInitialRotation,
            rot
        );
    });

    // Initial Rotation Rand
    connect(&mInitRotRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto rot = deg2idxVec(mInitRotRandSpinBox.getVector());
        setEmitterProperty(
            "Set Child Initial Rotation Random",
            "SetChildInitRotationRand",
            &Ptcl::Emitter::childInitialRotationRandom,
            &Ptcl::Emitter::setChildInitialRotationRandom,
            rot
        );
    });

    // Rotation Speed
    connect(&mRotVelSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto speed = deg2idxVec(mRotVelSpinBox.getVector());
        setEmitterProperty(
            "Set Child Rotation Speed",
            "SetChildRotVel",
            &Ptcl::Emitter::childRotationVelocity,
            &Ptcl::Emitter::setChildRotationVelocity,
            speed
        );
    });

    // Rotation Speed Rand
    connect(&mRotVelRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto vel = deg2idxVec(mRotVelRandSpinBox.getVector());
        setEmitterProperty(
            "Set Child Rotation Speed Rand",
            "SetChildRotVelRand",
            &Ptcl::Emitter::childRotationVelocityRandom,
            &Ptcl::Emitter::setChildRotationVelocityRandom,
            vel
        );
    });

    // Rotation Pivot
    connect(&mRotBasisSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto pivot = mRotBasisSpinBox.getVector();
        setEmitterProperty(
            "Set Child Rotation Pivot",
            "SetChildRotBasis",
            &Ptcl::Emitter::childRotationBasis,
            &Ptcl::Emitter::setChildRotationBasis,
            pivot
        );
    });
}

void ChildRotationInspector::populateProperties() {
    QSignalBlocker b1(mRotTypeSpinBox);
    QSignalBlocker b2(mInitRotSpinBox);
    QSignalBlocker b3(mInitRotRandSpinBox);
    QSignalBlocker b4(mRotVelSpinBox);
    QSignalBlocker b5(mRotVelRandSpinBox);
    QSignalBlocker b6(mRotBasisSpinBox);
    QSignalBlocker b7(mInheritRotCheckBox);

    auto idx2degVec = [](const Math::Vector3i& v) {
        return Math::Vector3f {
            Math::Util::to180(Math::Util::idx2deg(v.getX())),
            Math::Util::to180(Math::Util::idx2deg(v.getY())),
            Math::Util::to180(Math::Util::idx2deg(v.getZ()))
        };
    };

    mRotTypeSpinBox.setCurrentEnum(mEmitter->childRotationType());
    mInitRotSpinBox.setVector(idx2degVec(mEmitter->childInitialRotation()));
    mInitRotRandSpinBox.setVector(idx2degVec(mEmitter->childInitialRotationRandom()));
    mRotVelSpinBox.setVector(idx2degVec(mEmitter->childRotationVelocity()));
    mRotVelRandSpinBox.setVector(idx2degVec(mEmitter->childRotationVelocityRandom()));
    mRotBasisSpinBox.setVector(mEmitter->childRotationBasis());
    mInheritRotCheckBox.setChecked(mEmitter->isChildInheritRotation());

    updateAxis();
}

void ChildRotationInspector::updateAxis() {
    using Axis = VectorSpinBoxBase::Axis;

    switch (mEmitter->childRotationType()) {
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
