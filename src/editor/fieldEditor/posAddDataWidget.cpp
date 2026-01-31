#include "editor/fieldEditor/posAddDataWidget.h"

#include "math/util.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


FieldEditorWidget::PosAddDataWidget::PosAddDataWidget(QWidget* parent) :
    QWidget{parent} {

    // TODO: Set better limits?
    mPosSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Add-on value:", &mPosSpinBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Add to Position:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldEditorWidget::PosAddDataWidget::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        emit isEnabledUpdated(checked);
        mControlsWidget->setEnabled(checked);
    });

    // Pos Add
    connect(&mPosSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mData.posAdd = mPosSpinBox.getVector();
        emit dataUpdated(mData);
    });
}

void FieldEditorWidget::PosAddDataWidget::setData(const Ptcl::FieldData::FieldPosAddData& data, bool isEnabled) {
    QSignalBlocker b1(mPosSpinBox);
    QSignalBlocker b2(mEnabledCheckBox);

    mData = data;

    mPosSpinBox.setVector(mData.posAdd);
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
