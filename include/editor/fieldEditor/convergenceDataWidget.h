#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/fieldEditor/fieldEditorWidget.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldEditorWidget::ConvergenceDataWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ConvergenceDataWidget(QWidget* parent = nullptr);

    void setData(const Ptcl::FieldData::FieldConvergenceData& data, bool isEnabled);

signals:
    void dataUpdated(const Ptcl::FieldData::FieldConvergenceData& data);
    void isEnabledUpdated(bool isEnabled);

private:
    void setupConnections();

private:
    Ptcl::FieldData::FieldConvergenceData mData{};

    QWidget* mControlsWidget{nullptr};
    EnumComboBox<Ptcl::FieldConvergenceType> mTypeSpinBox{};
    VectorSpinBox<Math::Vector3f> mPosSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
