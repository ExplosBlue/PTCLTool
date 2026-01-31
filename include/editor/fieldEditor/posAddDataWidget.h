#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/fieldEditor/fieldEditorWidget.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldEditorWidget::PosAddDataWidget final : public QWidget {
    Q_OBJECT
public:
    explicit PosAddDataWidget(QWidget* parent = nullptr);

    void setData(const Ptcl::FieldData::FieldPosAddData& data, bool isEnabled);

signals:
    void dataUpdated(const Ptcl::FieldData::FieldPosAddData& data);
    void isEnabledUpdated(bool isEnabled);

private:
    void setupConnections();

private:
    Ptcl::FieldData::FieldPosAddData mData{};

    QWidget* mControlsWidget{nullptr};
    VectorSpinBox<Math::Vector3f> mPosSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
