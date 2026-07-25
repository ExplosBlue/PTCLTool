#include "editor/inspector/child/childGeneralInspector.h"

#include <QFormLayout>
#include <QVBoxLayout>

namespace PtclEditor {


// ========================================================================== //


ChildGeneralInspector::ChildGeneralInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    for (Ptcl::BillboardType type : sChildBillboardTypes) {
        mBillboardComboBox.addItem(Ptcl::toString(type), QVariant::fromValue(type));
    }

    mEnabledCheckBox.setText("Enable Child Emitter");
    mEnabledCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mEnabledCheckBox.setToolTip("Enables or disables the child emitter.");
    mFollowCheckBox.setText("Follow Parent Emitter");
    mFollowCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mFollowCheckBox.setToolTip("When enabled, the child emitter follows the parent emitter's position.");
    mParentFieldCheckBox.setText("Apply Parent's Field");
    mParentFieldCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mParentFieldCheckBox.setToolTip("When enabled, the parent's force fields affect the child emitter's particles.");

    mDrawOrderComboBox.addItem("Above Parent", QVariant::fromValue(DrawOrder::AboveParent));
    mDrawOrderComboBox.addItem("Below Parent", QVariant::fromValue(DrawOrder::BelowParent));

    auto* mainLayout = new QVBoxLayout(this);

    // General
    addSectionHeader(mainLayout, "General", this);

    auto* generalLayout = new QFormLayout;
    generalLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    generalLayout->addRow("Child:", &mEnabledCheckBox);
    mainLayout->addLayout(generalLayout);

    // Behavior
    addSectionHeader(mainLayout, "Behavior", this);

    auto* behaviorLayout = new QFormLayout;
    behaviorLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    behaviorLayout->addRow("Follow Type:", &mFollowCheckBox);
    behaviorLayout->addRow("Field Type:", &mParentFieldCheckBox);
    mainLayout->addLayout(behaviorLayout);

    // Rendering
    addSectionHeader(mainLayout, "Rendering", this);

    auto* renderingLayout = new QFormLayout;
    renderingLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    renderingLayout->addRow("Billboard Type:", &mBillboardComboBox);
    mBillboardComboBox.setToolTip("How child particles face the camera.");
    renderingLayout->addRow("Draw Order:", &mDrawOrderComboBox);
    mDrawOrderComboBox.setToolTip("Whether child particles are drawn above or below the parent.");
    mainLayout->addLayout(renderingLayout);

    mainLayout->addStretch();

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
