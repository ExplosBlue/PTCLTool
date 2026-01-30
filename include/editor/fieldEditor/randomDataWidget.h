#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/fieldEditor/fieldEditorWidget.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldEditorWidget::RandomDataWidget final : public QWidget {
    Q_OBJECT
public:
    explicit RandomDataWidget(QWidget* parent = nullptr);

    void setData(const Ptcl::FieldData::FieldRandomData& data, bool isEnabled);

signals:
    void dataUpdated(const Ptcl::FieldData::FieldRandomData& data);
    void isEnabledUpdated(bool isEnabled);

private:
    void setupConnections();

private:
    Ptcl::FieldData::FieldRandomData mData{};

    QWidget* mControlsWidget{nullptr};
    QSpinBox mRandomBlankSpinBox{};
    VectorSpinBox<Math::Vector3f> mRandomVelAddSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
