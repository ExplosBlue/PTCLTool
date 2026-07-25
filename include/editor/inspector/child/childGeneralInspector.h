#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
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
    enum class DrawOrder {
        AboveParent,
        BelowParent
    };

private:
    EnumComboBox<Ptcl::BillboardType> mBillboardComboBox{};
    QComboBox mDrawOrderComboBox{};
    QCheckBox mEnabledCheckBox{};
    QCheckBox mFollowCheckBox{};
    QCheckBox mParentFieldCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
