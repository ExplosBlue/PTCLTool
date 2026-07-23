#include "editor/inspector/child/childRotationScaleInspector.h"

#include "math/util.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


static const std::array rotTypeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::None,   "None",     "No rotation." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotX,   "X-Axis",   "Rotates around the X-axis." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotY,   "Y-Axis",   "Rotates around the Y-axis." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotZ,   "Z-Axis",   "Rotates around the Z-axis." },
    EnumOption<Ptcl::RotType>{ Ptcl::RotType::RotXYZ, "XYZ-Axis", "Rotates around all three axes." },
};


ChildRotationScaleInspector::ChildRotationScaleInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Rotation section
    addSectionHeader(mainLayout, "Rotation", this);

    mInheritRotCheckBox.setText("Inherit Parent Rotation");
    mInheritRotCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mainLayout->addRow("Inherit:", &mInheritRotCheckBox);
    mInheritRotCheckBox.setToolTip("Copies the parent's rotation.");

    mRotTypeComboBox.setOptions(rotTypeOptions);
    mRotTypeComboBox.setDescription("Which axis the particle rotates around.");
    mainLayout->addRow("Rotation Type:", &mRotTypeComboBox);
    mainLayout->addRow("Initial Rotation:", &mInitRotSpinBox);
    mInitRotSpinBox.setDecimals(2);
    mInitRotSpinBox.setSuffix(" °");
    mInitRotSpinBox.setToolTip("Starting rotation angle.");
    mainLayout->addRow("Initial Rotation Variation:", &mInitRotRandSpinBox);
    mInitRotRandSpinBox.setDecimals(2);
    mInitRotRandSpinBox.setSuffix(" °");
    mInitRotRandSpinBox.setToolTip("Random variation added to the starting rotation.");
    mainLayout->addRow("Rotation Speed:", &mRotVelSpinBox);
    mRotVelSpinBox.setDecimals(2);
    mRotVelSpinBox.setSuffix(" °");
    mRotVelSpinBox.setToolTip("How fast the particle rotates per frame.");
    mainLayout->addRow("Rotation Speed Variation:", &mRotVelRandSpinBox);
    mRotVelRandSpinBox.setDecimals(2);
    mRotVelRandSpinBox.setSuffix(" °");
    mRotVelRandSpinBox.setToolTip("Random variation added to the rotation speed.");
    mainLayout->addRow("Rotation Pivot Offset:", &mRotBasisSpinBox);
    mRotBasisSpinBox.setDecimals(2);
    mRotBasisSpinBox.setToolTip("Offset that shifts the center of rotation.");

    // Scale section
    addSectionHeader(mainLayout, "Scale", this);

    mInheritScaleCheckBox.setText("Inherit Parent Scale");
    mInheritScaleCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mainLayout->addRow("Inherit:", &mInheritScaleCheckBox);
    mInheritScaleCheckBox.setToolTip("Copies the parent's scale.");

    mInheritRateSpinBox.setRange(0.0, 100.0);
    mInheritRateSpinBox.setSuffix("%");
    mInheritRateSpinBox.setSingleStep(1.0);
    mInheritRateSpinBox.setDecimals(0);
    mainLayout->addRow("Inherit Rate:", &mInheritRateSpinBox);
    mInheritRateSpinBox.setToolTip("How much of the parent's scale is inherited.");

    mScaleSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mScaleSpinBox.setDecimals(2);
    mainLayout->addRow("Initial Scale:", &mScaleSpinBox);
    mScaleSpinBox.setToolTip("Starting scale when spawned.");

    mScaleTargetSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mScaleTargetSpinBox.setDecimals(2);
    mainLayout->addRow("Target Scale:", &mScaleTargetSpinBox);
    mScaleTargetSpinBox.setToolTip("Scale at the end of the animation.");

    mStartFrameSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mStartFrameSpinBox.setSuffix(" Frames");
    mainLayout->addRow("Start Frame:", &mStartFrameSpinBox);
    mStartFrameSpinBox.setToolTip("Frame when the scale animation starts.");

    setupConnections();
}

void ChildRotationScaleInspector::setupConnections() {
    auto deg2idxVec = [](const Math::Vector3f& v) {
        Math::Vector3i result {
            Math::Util::deg2idx(v.getX()),
            Math::Util::deg2idx(v.getY()),
            Math::Util::deg2idx(v.getZ())
        };
        return result;
    };

    // Rotation connections
    connect(&mInheritRotCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Child Inherit Rotation",
            "ToggleChildInheritRot",
            &Ptcl::Emitter::isChildInheritRotation,
            &Ptcl::Emitter::setChildInheritRotation,
            checked
        );
    });

    connect(&mRotTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mRotTypeComboBox.currentEnum();
        setEmitterProperty(
            "Set Child Rotation Type",
            "SetChildRotationType",
            &Ptcl::Emitter::childRotationType,
            &Ptcl::Emitter::setChildRotationType,
            type
        );
    });

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

    connect(&mInitRotRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto rot = deg2idxVec(mInitRotRandSpinBox.getVector());
        setEmitterProperty(
            "Set Child Initial Rotation Variation",
            "SetChildInitRotationRand",
            &Ptcl::Emitter::childInitialRotationRandom,
            &Ptcl::Emitter::setChildInitialRotationRandom,
            rot
        );
    });

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

    connect(&mRotVelRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        const auto vel = deg2idxVec(mRotVelRandSpinBox.getVector());
        setEmitterProperty(
            "Set Child Rotation Speed Variation",
            "SetChildRotVelRand",
            &Ptcl::Emitter::childRotationVelocityRandom,
            &Ptcl::Emitter::setChildRotationVelocityRandom,
            vel
        );
    });

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

    // Scale connections
    connect(&mInheritScaleCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Child Inherit Scale",
            "ToggleChildInheritScale",
            &Ptcl::Emitter::isChildInheritScale,
            &Ptcl::Emitter::setChildInheritScale,
            checked
        );
    });

    connect(&mScaleSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto scale = mScaleSpinBox.getVector();
        setEmitterProperty(
            "Set Child Scale",
            "SetChildScale",
            &Ptcl::Emitter::childScale,
            &Ptcl::Emitter::setChildScale,
            scale
        );
    });

    connect(&mScaleTargetSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto target = mScaleTargetSpinBox.getVector();
        setEmitterProperty(
            "Set Child Target Scale",
            "SetChildScaleTarget",
            &Ptcl::Emitter::childScaleTarget,
            &Ptcl::Emitter::setChildScaleTarget,
            target
        );
    });

    connect(&mInheritRateSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Scale Inherit Rate",
            "SetChildScaleInheritRate",
            &Ptcl::Emitter::childScaleInheritRate,
            &Ptcl::Emitter::setChildScaleInheritRate,
            static_cast<f32>(value / 100.0)
        );
    });

    connect(&mStartFrameSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Child Scale Start Frame",
            "SetChildScaleStartFrame",
            &Ptcl::Emitter::childScaleStartFrame,
            &Ptcl::Emitter::setChildScaleStartFrame,
            value
        );
    });
}

void ChildRotationScaleInspector::populateProperties() {
    QSignalBlocker b1(mRotTypeComboBox);
    QSignalBlocker b2(mInitRotSpinBox);
    QSignalBlocker b3(mInitRotRandSpinBox);
    QSignalBlocker b4(mRotVelSpinBox);
    QSignalBlocker b5(mRotVelRandSpinBox);
    QSignalBlocker b6(mRotBasisSpinBox);
    QSignalBlocker b7(mInheritRotCheckBox);
    QSignalBlocker b8(mScaleSpinBox);
    QSignalBlocker b9(mScaleTargetSpinBox);
    QSignalBlocker b10(mInheritRateSpinBox);
    QSignalBlocker b11(mStartFrameSpinBox);
    QSignalBlocker b12(mInheritScaleCheckBox);

    auto idx2degVec = [](const Math::Vector3i& v) {
        return Math::Vector3f {
            Math::Util::to180(Math::Util::idx2deg(v.getX())),
            Math::Util::to180(Math::Util::idx2deg(v.getY())),
            Math::Util::to180(Math::Util::idx2deg(v.getZ()))
        };
    };

    mRotTypeComboBox.setCurrentEnum(mEmitter->childRotationType());
    mInitRotSpinBox.setVector(idx2degVec(mEmitter->childInitialRotation()));
    mInitRotRandSpinBox.setVector(idx2degVec(mEmitter->childInitialRotationRandom()));
    mRotVelSpinBox.setVector(idx2degVec(mEmitter->childRotationVelocity()));
    mRotVelRandSpinBox.setVector(idx2degVec(mEmitter->childRotationVelocityRandom()));
    mRotBasisSpinBox.setVector(mEmitter->childRotationBasis());
    mInheritRotCheckBox.setChecked(mEmitter->isChildInheritRotation());

    updateAxis();

    // Scale properties
    mScaleSpinBox.setVector(mEmitter->childScale());
    mScaleTargetSpinBox.setVector(mEmitter->childScaleTarget());
    mInheritRateSpinBox.setValue(mEmitter->childScaleInheritRate() * 100.0);
    mStartFrameSpinBox.setValue(mEmitter->childScaleStartFrame());
    mInheritScaleCheckBox.setChecked(mEmitter->isChildInheritScale());
}

void ChildRotationScaleInspector::updateAxis() {
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
