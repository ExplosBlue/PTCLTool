#include "editor/childEditor/alphaPropertiesWidget.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::AlphaPropertiesWidget::AlphaPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    mAlphaSpinBox.setRange(0.0f, 1.0f);
    mAlphaSpinBox.setSingleStep(0.1f);

    mAlphaTargetSpinBox.setRange(0.0f, 1.0f);
    mAlphaTargetSpinBox.setSingleStep(0.1f);

    mAlphaInitSpinBox.setRange(0.0f, 1.0f);
    mAlphaInitSpinBox.setSingleStep(0.1f);

    mStartFrameSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mBaseFrameSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mInheritAlphaCheckBox.setText("Inherit Parent Alpha");

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Alpha:", &mInheritAlphaCheckBox);
    mainLayout->addRow("Alpha:", &mAlphaSpinBox);
    mainLayout->addRow("Alpha Target:", &mAlphaTargetSpinBox);
    mainLayout->addRow("Alpha Init:", &mAlphaInitSpinBox);
    mainLayout->addRow("Start Frame:", &mStartFrameSpinBox);
    mainLayout->addRow("Base Frame:", &mBaseFrameSpinBox);

    setLayout(mainLayout);
    setupConnections();
}

void ChildEditorWidget::AlphaPropertiesWidget::setupConnections() {
    // Inherit Alpha
    connect(&mInheritAlphaCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        emit inheritAlphaUpdated(checked);
    });

    // Alpha
    connect(&mAlphaSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.alpha = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });

    // Alpha Target
    connect(&mAlphaTargetSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.alphaTarget = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });

    // Alpha Init
    connect(&mAlphaInitSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.alphaInit = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });

    // Start Frame
    connect(&mStartFrameSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mProps.alphaStartFrame = value;
        emit propertiesUpdated(mProps);
    });

    // Base Frame
    connect(&mBaseFrameSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mProps.alphaBaseFrame = value;
        emit propertiesUpdated(mProps);
    });
}

void ChildEditorWidget::AlphaPropertiesWidget::setProperties(const Ptcl::ChildData::AlphaProperties& properties, bool inheritAlpha) {
    QSignalBlocker b1(mAlphaSpinBox);
    QSignalBlocker b2(mAlphaTargetSpinBox);
    QSignalBlocker b3(mAlphaInitSpinBox);
    QSignalBlocker b4(mStartFrameSpinBox);
    QSignalBlocker b5(mBaseFrameSpinBox);
    QSignalBlocker b6(mInheritAlphaCheckBox);

    mProps = properties;

    mAlphaSpinBox.setValue(mProps.alpha);
    mAlphaTargetSpinBox.setValue(mProps.alphaTarget);
    mAlphaInitSpinBox.setValue(mProps.alphaInit);
    mStartFrameSpinBox.setValue(mProps.alphaStartFrame);
    mBaseFrameSpinBox.setValue(mProps.alphaBaseFrame);

    mInheritAlphaCheckBox.setChecked(inheritAlpha);
}


// ========================================================================== //


} // namespace PtclEditor
