#include "editor/childEditor/basicPropertiesWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::BasicPropertiesWidget::BasicPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    // Billboard Type
    mainLayout->addRow("Billboard Type:", &mBillboardComboBox);
    connect(&mBillboardComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mBillboardComboBox.currentEnum();

        mProps.billboardType = type;
        // TODO:
        // mProps.isPolygon = (type == Ptcl::BillboardType::PolygonXY || type == Ptcl::BillboardType::PolygonXZ);
        // mProps.isVelLook = (type == Ptcl::BillboardType::VelLook || type == Ptcl::BillboardType::VelLookPolygon);
        // mProps.isEmitterBillboardMtx = mProps.isVelLook;
        emit propertiesUpdated(mProps);
    });
}

void ChildEditorWidget::BasicPropertiesWidget::setProperties(const Ptcl::ChildData::BasicProperties& properties) {
    QSignalBlocker b1(mBillboardComboBox);
    mProps = properties;
    mBillboardComboBox.setCurrentEnum(mProps.billboardType);
}


// ========================================================================== //


} // namespace PtclEditor
