#include "editor/inspector/child/childScaleInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


ChildScaleInspector::ChildScaleInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);

    mInheritScaleCheckBox.setText("Inherit Parent Scale");
    mInheritRateSpinBox.setRange(0.0f, 1.0f);
    mInheritRateSpinBox.setSingleStep(0.1f);
    mScaleSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mScaleTargetSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mStartFrameSpinBox.setRange(0, std::numeric_limits<s32>::max());

    mainLayout->addRow("Scale:", &mInheritScaleCheckBox);
    mainLayout->addRow("Inherit Rate:", &mInheritRateSpinBox);
    mainLayout->addRow("Initial Scale:", &mScaleSpinBox);
    mainLayout->addRow("Target Scale:", &mScaleTargetSpinBox);
    mainLayout->addRow("Start Frame:", &mStartFrameSpinBox);

    setupConnections();
}

void ChildScaleInspector::setupConnections() {
    // Inherit Scale
    connect(&mInheritScaleCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Child Inherit Scale",
            "ToggleChildInheritScale",
            &Ptcl::Emitter::isChildInheritScale,
            &Ptcl::Emitter::setChildInheritScale,
            checked
        );
    });

    // Scale
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

    // Scale Target
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

    // Inherit Rate
    connect(&mInheritRateSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Child Scale Inherit Rate",
            "SetChildScaleInheritRate",
            &Ptcl::Emitter::childScaleInheritRate,
            &Ptcl::Emitter::setChildScaleInheritRate,
            static_cast<f32>(value)
        );
    });

    // Start Frame
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

void ChildScaleInspector::populateProperties() {
    QSignalBlocker b1(mScaleSpinBox);
    QSignalBlocker b2(mScaleTargetSpinBox);
    QSignalBlocker b3(mInheritRateSpinBox);
    QSignalBlocker b4(mStartFrameSpinBox);
    QSignalBlocker b5(mInheritScaleCheckBox);

    mScaleSpinBox.setVector(mEmitter->childScale());
    mScaleTargetSpinBox.setVector(mEmitter->childScaleTarget());
    mInheritRateSpinBox.setValue(mEmitter->childScaleInheritRate());
    mStartFrameSpinBox.setValue(mEmitter->childScaleStartFrame());
    mInheritScaleCheckBox.setChecked(mEmitter->isChildInheritScale());
}


// ========================================================================== //


} // namespace PtclEditor
