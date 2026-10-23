#include "editor/preferencesDialog.h"

#include "util/settingsUtil.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>


namespace PtclTool {


// ========================================================================== //


PreferencesDialog::PreferencesDialog(QWidget* parent) :
    QDialog{parent} {
    setWindowTitle("Preferences");
    setModal(true);
    setMinimumWidth(480);

    auto* viewportGroup = new QGroupBox("Viewport", this);
    auto* viewportLayout = new QFormLayout(viewportGroup);
    viewportLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    viewportLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    viewportLayout->setHorizontalSpacing(12);
    viewportLayout->setVerticalSpacing(8);

    mBackgroundColorButton = new QPushButton(viewportGroup);
    mBackgroundColorButton->setMinimumWidth(140);
    mBackgroundColorButton->setToolTip("Viewport background color.");
    mBackgroundColorButton->setAutoDefault(false);
    connect(mBackgroundColorButton, &QPushButton::clicked, this, &PreferencesDialog::chooseBackgroundColor);
    viewportLayout->addRow("Background color:", mBackgroundColorButton);

    mGridCellSizeSpinBox = new QDoubleSpinBox(viewportGroup);
    mGridCellSizeSpinBox->setRange(SettingsUtil::cViewportGridCellSizeMin, SettingsUtil::cViewportGridCellSizeMax);
    mGridCellSizeSpinBox->setDecimals(2);
    mGridCellSizeSpinBox->setSingleStep(1.0);
    mGridCellSizeSpinBox->setSuffix(QStringLiteral(" units"));
    mGridCellSizeSpinBox->setToolTip("Size of grid cells.");
    viewportLayout->addRow("Grid cell size:", mGridCellSizeSpinBox);

    mGridVisibleCheckBox = new QCheckBox("Show grid", viewportGroup);
    mGridVisibleCheckBox->setToolTip("Show or hide the viewport grid.");
    viewportLayout->addRow(mGridVisibleCheckBox);

    auto* simulationGroup = new QGroupBox("Simulation", this);
    auto* simulationLayout = new QFormLayout(simulationGroup);
    simulationLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    simulationLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    simulationLayout->setHorizontalSpacing(12);
    simulationLayout->setVerticalSpacing(8);

    mRegularParticleCapacityComboBox = new QComboBox(simulationGroup);
    for (s32 capacity = SettingsUtil::cRegularParticleCapacityMin; capacity <= SettingsUtil::cRegularParticleCapacityMax; capacity *= 2) {
        mRegularParticleCapacityComboBox->addItem(QString::number(capacity), capacity);
    }
    mRegularParticleCapacityComboBox->setToolTip("Maximum number of standard particles.");
    simulationLayout->addRow("Standard particle limit:", mRegularParticleCapacityComboBox);

    mCompactParticleCapacityComboBox = new QComboBox(simulationGroup);
    for (s32 capacity = SettingsUtil::cCompactParticleCapacityMin; capacity <= SettingsUtil::cCompactParticleCapacityMax; capacity *= 2) {
        mCompactParticleCapacityComboBox->addItem(QString::number(capacity), capacity);
    }
    mCompactParticleCapacityComboBox->setToolTip("Maximum number of compact particles.");
    simulationLayout->addRow("Compact particle limit:", mCompactParticleCapacityComboBox);

    auto* note = new QLabel("Changing particle limits restarts the current preview simulation.", this);
    note->setWordWrap(true);

    auto* buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok
            | QDialogButtonBox::Cancel
            | QDialogButtonBox::Apply
            | QDialogButtonBox::RestoreDefaults,
        this
    );
    buttonBox->button(QDialogButtonBox::RestoreDefaults)->setText("Restore Defaults");
    buttonBox->button(QDialogButtonBox::Apply)->setToolTip("Save and apply changes without closing the dialog.");
    buttonBox->button(QDialogButtonBox::Ok)->setToolTip("Save and apply changes, then close the dialog.");

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(12);
    layout->addWidget(viewportGroup);
    layout->addWidget(simulationGroup);
    layout->addWidget(note);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, [this] {
        apply();
        accept();
    });
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &PreferencesDialog::apply);
    connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &PreferencesDialog::resetToDefaults);

    loadFromSettings();
}

void PreferencesDialog::chooseBackgroundColor() {
    const QColor color = QColorDialog::getColor(
        mBackgroundColor,
        this,
        "Viewport Background Color"
    );

    if (!color.isValid()) {
        return;
    }

    mBackgroundColor = color;
    updateBackgroundColorButton();
}

void PreferencesDialog::apply() {
    mGridCellSizeSpinBox->interpretText();

    const s32 regularCapacity = mRegularParticleCapacityComboBox->currentData().isValid()
        ? mRegularParticleCapacityComboBox->currentData().toInt()
        : SettingsUtil::cRegularParticleCapacityDefault;

    const s32 compactCapacity = mCompactParticleCapacityComboBox->currentData().isValid()
        ? mCompactParticleCapacityComboBox->currentData().toInt()
        : SettingsUtil::cCompactParticleCapacityDefault;

    SettingsUtil::setViewportBackgroundColor(mBackgroundColor);
    SettingsUtil::setViewportGridCellSize(static_cast<f32>(mGridCellSizeSpinBox->value()));
    SettingsUtil::setViewportGridVisible(mGridVisibleCheckBox->isChecked());
    SettingsUtil::setRegularParticleCapacity(regularCapacity);
    SettingsUtil::setCompactParticleCapacity(compactCapacity);

    emit settingsApplied();
}

void PreferencesDialog::resetToDefaults() {
    mBackgroundColor = SettingsUtil::defaultViewportBackgroundColor();
    mGridCellSizeSpinBox->setValue(SettingsUtil::cViewportGridCellSizeDefault);
    mGridVisibleCheckBox->setChecked(SettingsUtil::cViewportGridVisibleDefault);

    s32 regularIndex = mRegularParticleCapacityComboBox->findData(SettingsUtil::cRegularParticleCapacityDefault);
    if (regularIndex < 0) {
        regularIndex = 0;
    }
    mRegularParticleCapacityComboBox->setCurrentIndex(regularIndex);

    s32 compactIndex = mCompactParticleCapacityComboBox->findData(SettingsUtil::cCompactParticleCapacityDefault);
    if (compactIndex < 0) {
        compactIndex = 0;
    }
    mCompactParticleCapacityComboBox->setCurrentIndex(compactIndex);

    updateBackgroundColorButton();
}

void PreferencesDialog::loadFromSettings() {
    mBackgroundColor = SettingsUtil::viewportBackgroundColor();
    mGridCellSizeSpinBox->setValue(SettingsUtil::viewportGridCellSize());
    mGridVisibleCheckBox->setChecked(SettingsUtil::viewportGridVisible());

    s32 regularIndex = mRegularParticleCapacityComboBox->findData(
        SettingsUtil::regularParticleCapacity()
    );
    if (regularIndex < 0) {
        regularIndex = 0;
    }
    mRegularParticleCapacityComboBox->setCurrentIndex(regularIndex);

    s32 compactIndex = mCompactParticleCapacityComboBox->findData(
        SettingsUtil::compactParticleCapacity()
    );
    if (compactIndex < 0) {
        compactIndex = 0;
    }
    mCompactParticleCapacityComboBox->setCurrentIndex(compactIndex);

    updateBackgroundColorButton();
}

void PreferencesDialog::updateBackgroundColorButton() {
    QPixmap swatch(30, 18);
    swatch.fill(mBackgroundColor);

    QPainter painter(&swatch);
    const QColor borderColor = (mBackgroundColor.lightness() > 127)
        ? QColor(0, 0, 0, 90)
        : QColor(255, 255, 255, 90);
    painter.setPen(borderColor);
    painter.drawRect(swatch.rect().adjusted(0, 0, -1, -1));
    painter.end();

    mBackgroundColorButton->setIcon(QIcon(swatch));
    mBackgroundColorButton->setIconSize(swatch.size());
    mBackgroundColorButton->setText(mBackgroundColor.name(QColor::HexRgb).toUpper());
}


// ========================================================================== //


} // namespace PtclTool
