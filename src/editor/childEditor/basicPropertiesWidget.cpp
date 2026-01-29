#include "editor/childEditor/basicPropertiesWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::BasicPropertiesWidget::BasicPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    for (Ptcl::BillboardType type : sChildBillboardTypes) {
        mBillboardComboBox.addItem(Ptcl::toString(type), QVariant::fromValue(type));
    }

    mFollowCheckBox.setText("Follow Parent Emitter");
    mParentFieldCheckBox.setText("Apply Parent's Field");

    mDrawOrderComboBox.addItem("Above Parent", QVariant::fromValue(DrawOrder::AboveParent));
    mDrawOrderComboBox.addItem("Below Parent", QVariant::fromValue(DrawOrder::BelowParent));

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Billboard Type:", &mBillboardComboBox);
    mainLayout->addRow("Follow Type:", &mFollowCheckBox);
    mainLayout->addRow("Field Type:", &mParentFieldCheckBox);
    mainLayout->addRow("Draw Order:", &mDrawOrderComboBox);

    setupConnections();
}

void ChildEditorWidget::BasicPropertiesWidget::setupConnections() {
    // Is Follow
    connect(&mFollowCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        emit isFollowUpdated(checked);
    });

    // Is Parent Field
    connect(&mParentFieldCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        emit isParentFieldUpdated(checked);
    });

    // Billboard Type
    connect(&mBillboardComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mBillboardComboBox.currentData().value<Ptcl::BillboardType>();

        // TODO:
        // mProps.isVelLook = (type == Ptcl::BillboardType::VelLook || type == Ptcl::BillboardType::VelLookPolygon);
        // mProps.isEmitterBillboardMtx = mProps.isVelLook;

        mProps.billboardType = type;

        const bool isPolygon = (type == Ptcl::BillboardType::PolygonXY || type == Ptcl::BillboardType::PolygonXZ);
        emit isPolygonUpdated(isPolygon);

        emit propertiesUpdated(mProps);
    });

    // Draw Order
    connect(&mDrawOrderComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto order = mDrawOrderComboBox.currentData().value<DrawOrder>();
        const bool isPreDraw = (order == DrawOrder::BelowParent);
        emit isPreDrawUpdated(isPreDraw);
    });
}

void ChildEditorWidget::BasicPropertiesWidget::setProperties(const Ptcl::ChildData::BasicProperties& properties, bool isFollow, bool isParentField, bool isPreDraw) {
    QSignalBlocker b1(mBillboardComboBox);
    QSignalBlocker b2(mFollowCheckBox);
    QSignalBlocker b3(mParentFieldCheckBox);
    QSignalBlocker b4(mDrawOrderComboBox);

    mProps = properties;

    const s32 billboardIndex = mBillboardComboBox.findData(QVariant::fromValue(mProps.billboardType));
    mBillboardComboBox.setCurrentIndex(billboardIndex);

    mFollowCheckBox.setChecked(isFollow);
    mParentFieldCheckBox.setChecked(isParentField);

    const auto drawOrder = isPreDraw ? DrawOrder::BelowParent : DrawOrder::AboveParent;
    const s32 drawIndex = mDrawOrderComboBox.findData(QVariant::fromValue(drawOrder));
    mDrawOrderComboBox.setCurrentIndex(drawIndex);
}

void ChildEditorWidget::BasicPropertiesWidget::populateBillboardType() {
}


// ========================================================================== //


} // namespace PtclEditor
