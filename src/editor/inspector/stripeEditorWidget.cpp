#include "editor/inspector/stripeEditorWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


StripeEditorWidget::StripeEditorWidget(QWidget* parent) :
    InspectorWidgetBase{parent} {

    mTypeComboBox.addItem("Billboard Stripe", QVariant::fromValue(Ptcl::StripeType::Billboard));
    mTypeComboBox.addItem("Emitter Maxtrix", QVariant::fromValue(Ptcl::StripeType::EmitterMatrix));
    mTypeComboBox.addItem("Emitter UpDown", QVariant::fromValue(Ptcl::StripeType::EmitterUpDown));

    mNumHistSpinBox.setRange(2, 128);
    mStartAlphaSpinBox.setRange(0.0f, 1.0f);
    mEndAlphaSpinBox.setRange(0.0f, 1.0f);
    mUVScrollSpeedSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mHistStepSpinBox.setRange(0, 64);
    mDirIntepolateSpinBox.setRange(0.0f, 1.0f);
    mEmitterCoordCheckBox.setText("Perfectly follow emitter");

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Calculation Method:", &mTypeComboBox);
    mainLayout->addRow("Follow:", &mEmitterCoordCheckBox);
    mainLayout->addRow("History Size:", &mNumHistSpinBox);
    mainLayout->addRow("Start Point Alpha:", &mStartAlphaSpinBox);
    mainLayout->addRow("End Point Alpha:", &mEndAlphaSpinBox);
    mainLayout->addRow("UV Scroll Speed:", &mUVScrollSpeedSpinBox);
    mainLayout->addRow("History Spacing:", &mHistStepSpinBox);
    mainLayout->addRow("Direction Interpolation Ratio:", &mDirIntepolateSpinBox);

    setupConnections();
}

void StripeEditorWidget::setupConnections() {
    // Type
    connect(&mTypeComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto type = mTypeComboBox.currentData().value<Ptcl::StripeType>();
        setEmitterProperty(
            "Set Stripe Type",
            "SetStripeType",
            &Ptcl::Emitter::stripeType,
            &Ptcl::Emitter::setStripeType,
            type
        );
    });

    // Num Hist
    connect(&mNumHistSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Stripe History Size",
            "SetStripeNumHist",
            &Ptcl::Emitter::stripeNumHistory,
            &Ptcl::Emitter::setStripeNumHistory,
            value
        );
    });

    // Start Alpha
    connect(&mStartAlphaSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Stripe Start Alpha",
            "SetStripeStartAlpha",
            &Ptcl::Emitter::stripeStartAlpha,
            &Ptcl::Emitter::setStripeStartAlpha,
            static_cast<f32>(value)
        );
    });

    // End Alpha
    connect(&mEndAlphaSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Stripe End Alpha",
            "SetStripeEndAlpha",
            &Ptcl::Emitter::stripeEndAlpha,
            &Ptcl::Emitter::setStripeEndAlpha,
            static_cast<f32>(value)
        );
    });

    // UV Scroll Speed
    connect(&mUVScrollSpeedSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto speed = mUVScrollSpeedSpinBox.getVector();
        setEmitterProperty(
            "Set Stripe UV Scroll Speed",
            "SetStripeUVSpeed",
            &Ptcl::Emitter::stripeUVScrollSpeed,
            &Ptcl::Emitter::setStripeUVScrollSpeed,
            speed
        );
    });

    // Hist Step
    connect(&mHistStepSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Stripe History Spacing",
            "SetStripeHistStep",
            &Ptcl::Emitter::stripeHistoryStep,
            &Ptcl::Emitter::setStripeHistoryStep,
            value
        );
    });

    // Dir Interpolate
    connect(&mDirIntepolateSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Stripe Interpolation Ratio",
            "SetStripeDirInterp",
            &Ptcl::Emitter::stripeDirInterpolate,
            &Ptcl::Emitter::setStripeDirInterpolate,
            static_cast<f32>(value)
        );
    });

    // Emitter Coord
    connect(&mEmitterCoordCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Stripe Follow Emitter",
            "SetStripeEmitterCoord",
            &Ptcl::Emitter::isStripeEmitterCoord,
            &Ptcl::Emitter::setStripeEmitterCoord,
            checked
        );
    });
}

void StripeEditorWidget::populateProperties() {
    QSignalBlocker b1(mTypeComboBox);
    QSignalBlocker b2(mNumHistSpinBox);
    QSignalBlocker b3(mStartAlphaSpinBox);
    QSignalBlocker b4(mEndAlphaSpinBox);
    QSignalBlocker b5(mUVScrollSpeedSpinBox);
    QSignalBlocker b6(mHistStepSpinBox);
    QSignalBlocker b7(mDirIntepolateSpinBox);
    QSignalBlocker b8(mEmitterCoordCheckBox);

    const s32 typeIndex = mTypeComboBox.findData(QVariant::fromValue(mEmitter->stripeType()));
    mTypeComboBox.setCurrentIndex(typeIndex);

    mNumHistSpinBox.setValue(mEmitter->stripeNumHistory());
    mStartAlphaSpinBox.setValue(mEmitter->stripeStartAlpha());
    mEndAlphaSpinBox.setValue(mEmitter->stripeEndAlpha());
    mUVScrollSpeedSpinBox.setVector(mEmitter->stripeUVScrollSpeed());
    mHistStepSpinBox.setValue(mEmitter->stripeHistoryStep());
    mDirIntepolateSpinBox.setValue(mEmitter->stripeDirInterpolate());
    mEmitterCoordCheckBox.setChecked(mEmitter->isStripeEmitterCoord());
}


// ========================================================================== //


} // namespace PtclEditor
