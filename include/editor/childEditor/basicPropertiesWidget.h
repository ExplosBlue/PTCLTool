#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/childEditor/childEditorWidget.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget::BasicPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit BasicPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ChildData::BasicProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::ChildData::BasicProperties& properties);

private:
    Ptcl::ChildData::BasicProperties mProps{};

    EnumComboBox<Ptcl::BillboardType> mBillboardComboBox{};
};


// ========================================================================== //


} // namespace PtclEditor
