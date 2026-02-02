#include "editor/stripeEditorWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


StripeEditorWidget::StripeEditorWidget(QWidget* parent) :
    QWidget{parent} {

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
        const auto type = mTypeComboBox.currentData().value<Ptcl::StripeType>();
        mData.type = type;
        emit dataUpdated(mData);
    });

    // Num Hist
    connect(&mNumHistSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mData.numHistory = value;
        emit dataUpdated(mData);
    });

    // Start Alpha
    connect(&mStartAlphaSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mData.startAlpha = static_cast<f32>(value);
        emit dataUpdated(mData);
    });

    // End Alpha
    connect(&mEndAlphaSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mData.endAlpha = static_cast<f32>(value);
        emit dataUpdated(mData);
    });

    // UV Scroll Speed
    connect(&mUVScrollSpeedSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mData.uvScrollSpeed = mUVScrollSpeedSpinBox.getVector();
        emit dataUpdated(mData);
    });

    // Hist Step
    connect(&mHistStepSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mData.historyStep = value;
        emit dataUpdated(mData);
    });

    // Dir Interpolate
    connect(&mDirIntepolateSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mData.dirInterpolate = static_cast<f32>(value);
        emit dataUpdated(mData);
    });

    // Emitter Coord
    connect(&mEmitterCoordCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mStripeFlag.set(Ptcl::StripeFlag::EmitterCoord, checked);
        emit flagsUpdated(mStripeFlag);
    });
}

void StripeEditorWidget::setData(const Ptcl::StripeData& data, const BitFlag<Ptcl::StripeFlag>& stripeFlag) {
    QSignalBlocker b1(mTypeComboBox);
    QSignalBlocker b2(mNumHistSpinBox);
    QSignalBlocker b3(mStartAlphaSpinBox);
    QSignalBlocker b4(mEndAlphaSpinBox);
    QSignalBlocker b5(mUVScrollSpeedSpinBox);
    QSignalBlocker b6(mHistStepSpinBox);
    QSignalBlocker b7(mDirIntepolateSpinBox);
    QSignalBlocker b8(mEmitterCoordCheckBox);

    mData = data;
    mStripeFlag = stripeFlag;

    const s32 typeIndex = mTypeComboBox.findData(QVariant::fromValue(mData.type));
    mTypeComboBox.setCurrentIndex(typeIndex);

    mNumHistSpinBox.setValue(mData.numHistory);
    mStartAlphaSpinBox.setValue(mData.startAlpha);
    mEndAlphaSpinBox.setValue(mData.endAlpha);
    mUVScrollSpeedSpinBox.setVector(mData.uvScrollSpeed);
    mHistStepSpinBox.setValue(mData.historyStep);
    mDirIntepolateSpinBox.setValue(mData.dirInterpolate);
    mEmitterCoordCheckBox.setChecked(stripeFlag.isSet(Ptcl::StripeFlag::EmitterCoord));
}


// ========================================================================== //


} // namespace PtclEditor
