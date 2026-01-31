#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/fieldEditor/fieldEditorWidget.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FieldEditorWidget::MagnetDataWidget final : public QWidget {
    Q_OBJECT
public:
    explicit MagnetDataWidget(QWidget* parent = nullptr);

    void setData(const Ptcl::FieldData::FieldMagnetData& data, bool isEnabled);

signals:
    void dataUpdated(const Ptcl::FieldData::FieldMagnetData& data);
    void isEnabledUpdated(bool isEnabled);

private:
    void setupConnections();

private:
    Ptcl::FieldData::FieldMagnetData mData{};

    QWidget* mControlsWidget{nullptr};
    QDoubleSpinBox mMagnetPowerSpinBox{};
    VectorSpinBox<Math::Vector3f> mMagnetPosSpinBox{};
    QCheckBox mAxisXCheckBox{};
    QCheckBox mAxisYCheckBox{};
    QCheckBox mAxisZCheckBox{};
    QCheckBox mEnabledCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
