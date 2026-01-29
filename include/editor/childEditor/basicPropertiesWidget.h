#pragma once

#include "editor/childEditor/childEditorWidget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget::BasicPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit BasicPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ChildData::BasicProperties& properties, bool isFollow, bool isParentField);

signals:
    void propertiesUpdated(const Ptcl::ChildData::BasicProperties& properties);
    void isFollowUpdated(bool isFollow);
    void isParentFieldUpdated(bool isField);
    void isPolygonUpdated(bool isPolygon);

private:
    void setupConnections();
    void populateBillboardType();

private:
    static constexpr std::array<Ptcl::BillboardType, 6> sChildBillboardTypes{
        Ptcl::BillboardType::Billboard,
        Ptcl::BillboardType::PolygonXY,
        Ptcl::BillboardType::PolygonXZ,
        Ptcl::BillboardType::VelLook,
        Ptcl::BillboardType::VelLookPolygon,
        Ptcl::BillboardType::BillboardY,
    };

private:
    Ptcl::ChildData::BasicProperties mProps{};

    QComboBox mBillboardComboBox{};
    QCheckBox mFollowCheckBox{};
    QCheckBox mParentFieldCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
