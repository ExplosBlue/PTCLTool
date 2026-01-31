#include "editor/fieldEditor/magnetDataWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


FieldEditorWidget::MagnetDataWidget::MagnetDataWidget(QWidget* parent) :
    QWidget{parent} {

    // TODO: Better ranges?
    mMagnetPowerSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mMagnetPosSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Magnet Power:", &mMagnetPowerSpinBox);
    controlsLayout->addRow("Magnet Position:", &mMagnetPosSpinBox);
    controlsLayout->addRow("Target X-Axis:", &mAxisXCheckBox);
    controlsLayout->addRow("Target Y-Axis:", &mAxisYCheckBox);
    controlsLayout->addRow("Target Z-Axis:", &mAxisZCheckBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Magnetic Force:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldEditorWidget::MagnetDataWidget::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        emit isEnabledUpdated(checked);
        mControlsWidget->setEnabled(checked);
    });

    // Magnet Power
    connect(&mMagnetPowerSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mData.magnetPower = static_cast<f32>(value);
        emit dataUpdated(mData);
    });

    // Magnet pos
    connect(&mMagnetPosSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mData.magnetPos = mMagnetPosSpinBox.getVector();
        emit dataUpdated(mData);
    });

    // Axis X
    connect(&mAxisXCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mData.magnetFlag.set(Ptcl::FieldMagnetFlag::AxisTargetX, checked);
        emit dataUpdated(mData);
    });

    // Axis Y
    connect(&mAxisYCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mData.magnetFlag.set(Ptcl::FieldMagnetFlag::AxisTargetY, checked);
        emit dataUpdated(mData);
    });

    // Axis Z
    connect(&mAxisZCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mData.magnetFlag.set(Ptcl::FieldMagnetFlag::AxisTargetZ, checked);
        emit dataUpdated(mData);
    });
}

void FieldEditorWidget::MagnetDataWidget::setData(const Ptcl::FieldData::FieldMagnetData& data, bool isEnabled) {
    QSignalBlocker b1(mMagnetPowerSpinBox);
    QSignalBlocker b2(mMagnetPosSpinBox);
    QSignalBlocker b3(mAxisXCheckBox);
    QSignalBlocker b4(mAxisYCheckBox);
    QSignalBlocker b5(mAxisZCheckBox);
    QSignalBlocker b6(mEnabledCheckBox);

    mData = data;

    mMagnetPowerSpinBox.setValue(mData.magnetPower);
    mMagnetPosSpinBox.setVector(mData.magnetPos);
    mAxisXCheckBox.setChecked(mData.magnetFlag.isSet(Ptcl::FieldMagnetFlag::AxisTargetX));
    mAxisYCheckBox.setChecked(mData.magnetFlag.isSet(Ptcl::FieldMagnetFlag::AxisTargetY));
    mAxisZCheckBox.setChecked(mData.magnetFlag.isSet(Ptcl::FieldMagnetFlag::AxisTargetZ));
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
