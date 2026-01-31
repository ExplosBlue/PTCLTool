#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/fieldEditor/fieldEditorWidget.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldEditorWidget::SpinDataWidget final : public QWidget {
    Q_OBJECT
public:
    explicit SpinDataWidget(QWidget* parent = nullptr);

    void setData(const Ptcl::FieldData::FieldSpinData& data, bool isEnabled);

signals:
    void dataUpdated(const Ptcl::FieldData::FieldSpinData& data);
    void isEnabledUpdated(bool isEnabled);

private:
    void setupConnections();

private:
    Ptcl::FieldData::FieldSpinData mData{};

    QWidget* mControlsWidget{nullptr};
    QDoubleSpinBox mSpinRotateSpinBox{};
    EnumComboBox<Ptcl::FieldSpinAxis> mSpinAxisSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
