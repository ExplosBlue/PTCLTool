#include "editor/inspector/child/childGeneralInspector.h"

#include <QFormLayout>
#include <QVBoxLayout>

namespace PtclEditor {


// ========================================================================== //


static const std::array childBillboardTypeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::Billboard,      "Camera Facing",         "Particle always faces the camera on all axes." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::PolygonXY,      "Fixed (XY Plane)",      "Particle is fixed flat on the XY plane (vertical)." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::PolygonXZ,      "Fixed (XZ Plane)",      "Particle is fixed flat on the XZ plane (horizontal)." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::VelLook,        "Velocity Billboard",    "Particle rotates to face along its direction of travel." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::VelLookPolygon, "Velocity Polygon",      "Particle is fixed to the velocity direction using emitter orientation." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::BillboardY,     "Camera Facing (Y Axis)", "Particle rotates to face camera, but only around the Y axis." },
};


ChildGeneralInspector::ChildGeneralInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    mBillboardComboBox.setOptions(childBillboardTypeOptions);
    mBillboardComboBox.setDescription("How child particles face the camera.");

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
        const auto type = mBillboardComboBox.currentEnum();
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
    mBillboardComboBox.setCurrentEnum(mEmitter->childBillboardType());

    mEnabledCheckBox.setChecked(mEmitter->isChildEnabled());

    mFollowCheckBox.setChecked(mEmitter->isChildFollow());
    mParentFieldCheckBox.setChecked(mEmitter->isChildParentField());

    const auto drawOrder = mEmitter->isChildPreDraw() ? DrawOrder::BelowParent : DrawOrder::AboveParent;
    const s32 drawIndex = mDrawOrderComboBox.findData(QVariant::fromValue(drawOrder));
    mDrawOrderComboBox.setCurrentIndex(drawIndex);
}


// ========================================================================== //


} // namespace PtclEditor
