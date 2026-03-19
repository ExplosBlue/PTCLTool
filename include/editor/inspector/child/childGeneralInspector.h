#pragma once

#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QComboBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildGeneralInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ChildGeneralInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    static constexpr std::array<Ptcl::BillboardType, 6> sChildBillboardTypes{
        Ptcl::BillboardType::Billboard,
        Ptcl::BillboardType::PolygonXY,
        Ptcl::BillboardType::PolygonXZ,
        Ptcl::BillboardType::VelLook,
        Ptcl::BillboardType::VelLookPolygon,
        Ptcl::BillboardType::BillboardY,
    };

    enum class DrawOrder {
        AboveParent,
        BelowParent
    };

private:
    QComboBox mBillboardComboBox{};
    QComboBox mDrawOrderComboBox{};
    QCheckBox mEnabledCheckBox{};
    QCheckBox mFollowCheckBox{};
    QCheckBox mParentFieldCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
