#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/fieldEditor/fieldEditorWidget.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldEditorWidget::CollisionDataWidget final : public QWidget {
    Q_OBJECT
public:
    explicit CollisionDataWidget(QWidget* parent = nullptr);

    void setData(const Ptcl::FieldData::FieldCollisionData& data, bool isEnabled);

signals:
    void dataUpdated(const Ptcl::FieldData::FieldCollisionData& data);
    void isEnabledUpdated(bool isEnabled);

private:
    void setupConnections();

private:
    Ptcl::FieldData::FieldCollisionData mData{};

    QWidget* mControlsWidget{nullptr};
    EnumComboBox<Ptcl::FieldCollisionType> mCollisionTypeSpinBox{};
    QCheckBox mIsWorldCheckBox{};
    QDoubleSpinBox mCoefSpinBox{};
    QDoubleSpinBox mCoordSpinBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
