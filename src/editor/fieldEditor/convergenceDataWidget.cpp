#include "editor/fieldEditor/convergenceDataWidget.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


FieldEditorWidget::ConvergenceDataWidget::ConvergenceDataWidget(QWidget* parent) :
    QWidget{parent} {

    // TODO: Better ranges?
    mPosSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Convergence Position:", &mPosSpinBox);
    controlsLayout->addRow("Position Follow Type:", &mTypeSpinBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Convergence:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldEditorWidget::ConvergenceDataWidget::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        emit isEnabledUpdated(checked);
        mControlsWidget->setEnabled(checked);
    });

    // Type
    connect(&mTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        mData.convergenceType = mTypeSpinBox.currentEnum();
        emit dataUpdated(mData);
    });

    // Position
    connect(&mPosSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mData.convergencePos = mPosSpinBox.getVector();
        emit dataUpdated(mData);
    });
}

void FieldEditorWidget::ConvergenceDataWidget::setData(const Ptcl::FieldData::FieldConvergenceData& data, bool isEnabled) {
    QSignalBlocker b1(mTypeSpinBox);
    QSignalBlocker b2(mPosSpinBox);
    QSignalBlocker b3(mEnabledCheckBox);

    mData = data;

    mTypeSpinBox.setCurrentEnum(mData.convergenceType);
    mPosSpinBox.setVector(mData.convergencePos);
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
