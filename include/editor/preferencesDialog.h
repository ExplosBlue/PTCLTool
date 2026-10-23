#pragma once

#include <QColor>
#include <QDialog>


class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QPushButton;


// ========================================================================== //


namespace PtclTool {


// ========================================================================== //


class PreferencesDialog final : public QDialog {
    Q_OBJECT
public:
    explicit PreferencesDialog(QWidget* parent = nullptr);

signals:
    void settingsApplied();

private slots:
    void chooseBackgroundColor();
    void apply();
    void resetToDefaults();

private:
    void loadFromSettings();
    void updateBackgroundColorButton();

private:
    QPushButton* mBackgroundColorButton{nullptr};
    QDoubleSpinBox* mGridCellSizeSpinBox{nullptr};
    QCheckBox* mGridVisibleCheckBox{nullptr};
    QComboBox* mRegularParticleCapacityComboBox{nullptr};
    QComboBox* mCompactParticleCapacityComboBox{nullptr};

    QColor mBackgroundColor{};
};


// ========================================================================== //


} // namespace PtclTool
