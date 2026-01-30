#include "editor/fieldEditor/randomDataWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


FieldEditorWidget::RandomDataWidget::RandomDataWidget(QWidget* parent) :
    QWidget{parent} {

    // TODO: Better ranges?
    mRandomVelAddSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mRandomBlankSpinBox.setRange(std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Random Velocity Add:", &mRandomVelAddSpinBox);
    controlsLayout->addRow("Random Blank:", &mRandomBlankSpinBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Randomness:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldEditorWidget::RandomDataWidget::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        emit isEnabledUpdated(checked);
        mControlsWidget->setEnabled(checked);
    });

    // Random Vel Add
    connect(&mRandomVelAddSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mData.randomVelAdd = mRandomVelAddSpinBox.getVector();
        emit dataUpdated(mData);
    });

    // Random Blank
    connect(&mRandomBlankSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mData.randomBlank = value;
        emit dataUpdated(mData);
    });
}

void FieldEditorWidget::RandomDataWidget::setData(const Ptcl::FieldData::FieldRandomData& data, bool isEnabled) {
    QSignalBlocker b1(mRandomBlankSpinBox);
    QSignalBlocker b2(mRandomVelAddSpinBox);
    QSignalBlocker b3(mEnabledCheckBox);

    mData = data;

    mRandomBlankSpinBox.setValue(mData.randomBlank);
    mRandomVelAddSpinBox.setVector(mData.randomVelAdd);
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
