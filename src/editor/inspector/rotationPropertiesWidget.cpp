#include "editor/inspector/rotationPropertiesWidget.h"

#include "math/util.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


RotationPropertiesWidget::RotationPropertiesWidget(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Rotation Type", &mRotTypeSpinBox);
    mainLayout->addRow("Initial Rotation", &mInitRotSpinBox);
    mainLayout->addRow("Initial Rotation Random", &mInitRotRandSpinBox);
    mainLayout->addRow("Rotation Speed", &mRotVelSpinBox);
    mainLayout->addRow("Rotation Speed Random", &mRotVelRandSpinBox);
    mainLayout->addRow("Rotation Pivot Offset", &mRotBasisSpinBox);

    setLayout(mainLayout);
    setupConnections();
}

void RotationPropertiesWidget::setupConnections() {
    connect(&mRotTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mRotTypeSpinBox.currentEnum();

        setEmitterProperty(
            "Set Rotation type",
            "SetRotType",
            &Ptcl::Emitter::rotationType,
            &Ptcl::Emitter::setRotationType,
            type
        );
    });

    auto deg2idxVec = [](const Math::Vector3f& v) {
        Math::Vector3i result {
            Math::Util::deg2idx(v.getX()),
            Math::Util::deg2idx(v.getY()),
            Math::Util::deg2idx(v.getZ())
        };
        return result;
    };

    connect(&mInitRotSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto rotation = deg2idxVec(mInitRotSpinBox.getVector());

        setEmitterProperty(
            "Set Initial Rotation",
            "SetInitRot",
            &Ptcl::Emitter::initialRotation,
            &Ptcl::Emitter::setInitialRotation,
            rotation
        );
    });

    connect(&mInitRotRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto rand = deg2idxVec(mInitRotRandSpinBox.getVector());

        setEmitterProperty(
            "Set Initial Rotation Random",
            "SetInitRotRand",
            &Ptcl::Emitter::initialRotationRandom,
            &Ptcl::Emitter::setInitialRotationRandom,
            rand
        );
    });

    connect(&mRotVelSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto velocity = deg2idxVec(mRotVelSpinBox.getVector());

        setEmitterProperty(
            "Set Rotation Speed",
            "SetRotVel",
            &Ptcl::Emitter::rotationVelocity,
            &Ptcl::Emitter::setRotationVelocity,
            velocity
        );
    });

    connect(&mRotVelRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto rand = deg2idxVec(mRotVelRandSpinBox.getVector());

        setEmitterProperty(
            "Set Rotation Speed Random",
            "SetRotVelRand",
            &Ptcl::Emitter::rotationVelocityRandom,
            &Ptcl::Emitter::setRotationVelocityRandom,
            rand
        );
    });

    connect(&mRotBasisSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto basis = mRotBasisSpinBox.getVector();

        setEmitterProperty(
            "Set Rotation Pivot",
            "SetRotBasis",
            &Ptcl::Emitter::rotationBasis,
            &Ptcl::Emitter::setRotationBasis,
            basis
        );
    });
}


void RotationPropertiesWidget::populateProperties() {
    QSignalBlocker b1(mRotTypeSpinBox);
    QSignalBlocker b2(mInitRotSpinBox);
    QSignalBlocker b3(mInitRotRandSpinBox);
    QSignalBlocker b4(mRotVelSpinBox);
    QSignalBlocker b5(mRotVelRandSpinBox);
    QSignalBlocker b6(mRotBasisSpinBox);

    auto idx2degVec = [](const Math::Vector3i& v) {
        return Math::Vector3f {
            Math::Util::to180(Math::Util::idx2deg(v.getX())),
            Math::Util::to180(Math::Util::idx2deg(v.getY())),
            Math::Util::to180(Math::Util::idx2deg(v.getZ()))
        };
    };

    mRotTypeSpinBox.setCurrentEnum(mEmitter->rotationType());
    mInitRotSpinBox.setVector(idx2degVec(mEmitter->initialRotation()));
    mInitRotRandSpinBox.setVector(idx2degVec(mEmitter->initialRotationRandom()));
    mRotVelSpinBox.setVector(idx2degVec(mEmitter->rotationVelocity()));
    mRotVelRandSpinBox.setVector(idx2degVec(mEmitter->rotationVelocityRandom()));
    mRotBasisSpinBox.setVector(mEmitter->rotationBasis());

    updateAxis();
}

void RotationPropertiesWidget::updateAxis() {
    using Axis = VectorSpinBoxBase::Axis;

    switch (mEmitter->rotationType()) {
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
