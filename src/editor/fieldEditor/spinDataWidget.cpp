#include "editor/fieldEditor/spinDataWidget.h"

#include "math/util.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


FieldEditorWidget::SpinDataWidget::SpinDataWidget(QWidget* parent) :
    QWidget{parent} {

    mSpinRotateSpinBox.setRange(-180.0f, 180.0f);
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Spin Rotation:", &mSpinRotateSpinBox);
    controlsLayout->addRow("Spin Axis:", &mSpinAxisSpinBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Spin:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldEditorWidget::SpinDataWidget::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        emit isEnabledUpdated(checked);
        mControlsWidget->setEnabled(checked);
    });

    // Spin Rotate
    connect(&mSpinRotateSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mData.spinRotate = Math::Util::deg2idx(static_cast<f32>(value));
        emit dataUpdated(mData);
    });

    // Spin Axis
    connect(&mSpinAxisSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        mData.spinAxis = mSpinAxisSpinBox.currentEnum();
        emit dataUpdated(mData);
    });
}

void FieldEditorWidget::SpinDataWidget::setData(const Ptcl::FieldData::FieldSpinData& data, bool isEnabled) {
    QSignalBlocker b1(mSpinRotateSpinBox);
    QSignalBlocker b2(mSpinAxisSpinBox);
    QSignalBlocker b3(mEnabledCheckBox);

    mData = data;

    mSpinRotateSpinBox.setValue(Math::Util::to180(Math::Util::idx2deg(mData.spinRotate)));
    mSpinAxisSpinBox.setCurrentEnum(mData.spinAxis);
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
