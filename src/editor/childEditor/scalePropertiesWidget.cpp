#include "editor/childEditor/scalePropertiesWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::ScalePropertiesWidget::ScalePropertiesWidget(QWidget* parent) :
    QWidget{parent} {

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

void ChildEditorWidget::ScalePropertiesWidget::setupConnections() {
    // Inherit Scale
    connect(&mInheritScaleCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        emit inheritScaleUpdated(checked);
    });

    // Scale
    connect(&mScaleSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mProps.scale = mScaleSpinBox.getVector();
        emit propertiesUpdated(mProps);
    });

    // Scale Target
    connect(&mScaleTargetSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mProps.scaleTarget = mScaleTargetSpinBox.getVector();
        emit propertiesUpdated(mProps);
    });

    // Inherit Rate
    connect(&mInheritRateSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.scaleInheritRate = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });

    // Inherit Rate
    connect(&mStartFrameSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mProps.scaleStartFrame = value;
        emit propertiesUpdated(mProps);
    });
}

void ChildEditorWidget::ScalePropertiesWidget::setProperties(const Ptcl::ChildData::ScaleProperties& properties, bool inheritScale) {
    QSignalBlocker b1(mScaleSpinBox);
    QSignalBlocker b2(mScaleTargetSpinBox);
    QSignalBlocker b3(mInheritRateSpinBox);
    QSignalBlocker b4(mStartFrameSpinBox);
    QSignalBlocker b5(mInheritScaleCheckBox);

    mProps = properties;

    mScaleSpinBox.setVector(mProps.scale);
    mScaleTargetSpinBox.setVector(mProps.scaleTarget);
    mInheritRateSpinBox.setValue(mProps.scaleInheritRate);
    mStartFrameSpinBox.setValue(mProps.scaleStartFrame);
    mInheritScaleCheckBox.setChecked(inheritScale);
}


// ========================================================================== //


} // namespace PtclEditor
