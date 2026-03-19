#include "editor/inspector/child/childGeneralInspector.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


ChildGeneralInspector::ChildGeneralInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    for (Ptcl::BillboardType type : sChildBillboardTypes) {
        mBillboardComboBox.addItem(Ptcl::toString(type), QVariant::fromValue(type));
    }

    mEnabledCheckBox.setText("Enable Child Emitter");
    mFollowCheckBox.setText("Follow Parent Emitter");
    mParentFieldCheckBox.setText("Apply Parent's Field");

    mDrawOrderComboBox.addItem("Above Parent", QVariant::fromValue(DrawOrder::AboveParent));
    mDrawOrderComboBox.addItem("Below Parent", QVariant::fromValue(DrawOrder::BelowParent));

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Child:", &mEnabledCheckBox);
    mainLayout->addRow("Billboard Type:", &mBillboardComboBox);
    mainLayout->addRow("Follow Type:", &mFollowCheckBox);
    mainLayout->addRow("Field Type:", &mParentFieldCheckBox);
    mainLayout->addRow("Draw Order:", &mDrawOrderComboBox);

    setupConnections();
}

void ChildGeneralInspector::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Child",
            "ToggleChild",
            &Ptcl::Emitter::isChildEnabled,
            &Ptcl::Emitter::setChildEnabled,
            checked
            );
    });

    // Is Follow
    connect(&mFollowCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Child Follow Parent",
            "ToggleChildFollowParent",
            &Ptcl::Emitter::isChildFollow,
            &Ptcl::Emitter::setChildFollow,
            checked
        );
    });

    // Is Parent Field
    connect(&mParentFieldCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Child Apply Parent Field",
            "ToggleChildParentField",
            &Ptcl::Emitter::isChildParentField,
            &Ptcl::Emitter::setChildParentField,
            checked
        );
    });

    // Billboard Type
    connect(&mBillboardComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mBillboardComboBox.currentData().value<Ptcl::BillboardType>();
        setEmitterProperty(
            "Set Child Billboard Type",
            "SetChildBillboardType",
            &Ptcl::Emitter::childBillboardType,
            &Ptcl::Emitter::setChildBillboardType,
            type
        );
    });

    // Draw Order
    connect(&mDrawOrderComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto order = mDrawOrderComboBox.currentData().value<DrawOrder>();
        const bool isPreDraw = (order == DrawOrder::BelowParent);
        setEmitterProperty(
            "Set Child Draw Order",
            "SetChildDrawOrder",
            &Ptcl::Emitter::isChildPreDraw,
            &Ptcl::Emitter::setChildPreDraw,
            isPreDraw
        );
    });
}

void ChildGeneralInspector::populateProperties() {
    QSignalBlocker b1(mBillboardComboBox);
    QSignalBlocker b2(mFollowCheckBox);
    QSignalBlocker b3(mParentFieldCheckBox);
    QSignalBlocker b4(mDrawOrderComboBox);

    const s32 billboardIndex = mBillboardComboBox.findData(QVariant::fromValue(mEmitter->childBillboardType()));
    mBillboardComboBox.setCurrentIndex(billboardIndex);

    mEnabledCheckBox.setChecked(mEmitter->isChildEnabled());

    mFollowCheckBox.setChecked(mEmitter->isChildFollow());
    mParentFieldCheckBox.setChecked(mEmitter->isChildParentField());

    const auto drawOrder = mEmitter->isChildPreDraw() ? DrawOrder::BelowParent : DrawOrder::AboveParent;
    const s32 drawIndex = mDrawOrderComboBox.findData(QVariant::fromValue(drawOrder));
    mDrawOrderComboBox.setCurrentIndex(drawIndex);
}


// ========================================================================== //


} // namespace PtclEditor
