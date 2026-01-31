#include "editor/components/collapsibleWidget.h"
#include "editor/fieldEditor/collisionDataWidget.h"
#include "editor/fieldEditor/fieldEditorWidget.h"
#include "editor/fieldEditor/magnetDataWidget.h"
#include "editor/fieldEditor/randomDataWidget.h"
#include "editor/fieldEditor/spinDataWidget.h"

#include <QScrollArea>


namespace PtclEditor {


// ========================================================================== //


FieldEditorWidget::FieldEditorWidget(QWidget* parent) :
    QWidget{parent} {

    mRandomDataWidget = new RandomDataWidget(this);
    mMagnetDataWidget = new MagnetDataWidget(this);
    mSpinDataWidget = new SpinDataWidget(this);
    mCollisionDataWidget = new CollisionDataWidget(this);

    // Standard Widget
    auto* standardWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(standardWidget);
    standardWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    standardWidget->setMinimumWidth(400);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(standardWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->addWidget(scrollArea);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(outerLayout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setupLayout(mainLayout);
    setupConnections();
}

void FieldEditorWidget::setupLayout(QVBoxLayout* mainLayout) {
    // Section Containers
    mSectionsContainer = new QWidget(this);
    auto* sectionsLayout = new QVBoxLayout(mSectionsContainer);
    sectionsLayout->setContentsMargins(0, 0, 0, 0);

    auto addSection = [sectionsLayout](const QString& title, QWidget* widget) {
        auto* section = new CollapsibleWidget(title);
        section->setContent(widget);
        sectionsLayout->addWidget(section);
    };

    addSection("Randomness", mRandomDataWidget);
    addSection("Magnetic Force", mMagnetDataWidget);
    addSection("Spin Force", mSpinDataWidget);
    addSection("Collision Plane", mCollisionDataWidget);

    sectionsLayout->addStretch();

    mainLayout->addWidget(mSectionsContainer);
    mainLayout->addStretch();
}

void FieldEditorWidget::setupConnections() {
    // Randomness
    connect(mRandomDataWidget, &RandomDataWidget::dataUpdated, this, [this](const Ptcl::FieldData::FieldRandomData& data) {
        if (!mDataPtr) { return; }
        mDataPtr->setRandomData(data);
    });

    connect(mRandomDataWidget, &RandomDataWidget::isEnabledUpdated, this, [this](bool isEnabled) {
        if (!mDataPtr) { return; }
        mFieldFlag.set(Ptcl::FieldFlag::Random, isEnabled);
        emit flagsUpdated(mFieldFlag);
    });

    // Magnet
    connect(mMagnetDataWidget, &MagnetDataWidget::dataUpdated, this, [this](const Ptcl::FieldData::FieldMagnetData& data) {
        if (!mDataPtr) { return; }
        mDataPtr->setMagnetData(data);
    });

    connect(mMagnetDataWidget, &MagnetDataWidget::isEnabledUpdated, this, [this](bool isEnabled) {
        if (!mDataPtr) { return; }
        mFieldFlag.set(Ptcl::FieldFlag::Magnet, isEnabled);
        emit flagsUpdated(mFieldFlag);
    });

    // Spin
    connect(mSpinDataWidget, &SpinDataWidget::dataUpdated, this, [this](const Ptcl::FieldData::FieldSpinData& data) {
        if (!mDataPtr) { return; }
        mDataPtr->setSpinData(data);
    });

    connect(mSpinDataWidget, &SpinDataWidget::isEnabledUpdated, this, [this](bool isEnabled) {
        if (!mDataPtr) { return; }
        mFieldFlag.set(Ptcl::FieldFlag::Spin, isEnabled);
        emit flagsUpdated(mFieldFlag);
    });

    // Collision
    connect(mCollisionDataWidget, &CollisionDataWidget::dataUpdated, this, [this](const Ptcl::FieldData::FieldCollisionData& data) {
        if (!mDataPtr) { return; }
        mDataPtr->setCollisionData(data);
    });

    connect(mCollisionDataWidget, &CollisionDataWidget::isEnabledUpdated, this, [this](bool isEnabled) {
        if (!mDataPtr) { return; }
        mFieldFlag.set(Ptcl::FieldFlag::Collision, isEnabled);
        emit flagsUpdated(mFieldFlag);
    });
}

void FieldEditorWidget::setData(Ptcl::FieldData* fieldData, const BitFlag<Ptcl::FieldFlag>& fieldFlag) {
    QSignalBlocker b1(mRandomDataWidget);
    QSignalBlocker b2(mMagnetDataWidget);
    QSignalBlocker b3(mSpinDataWidget);
    QSignalBlocker b5(mCollisionDataWidget);

    mDataPtr = fieldData;
    mFieldFlag = fieldFlag;

    mRandomDataWidget->setData(mDataPtr->randomData(), mFieldFlag.isSet(Ptcl::FieldFlag::Random));
    mMagnetDataWidget->setData(mDataPtr->magnetData(), mFieldFlag.isSet(Ptcl::FieldFlag::Magnet));
    mSpinDataWidget->setData(mDataPtr->spinData(), mFieldFlag.isSet(Ptcl::FieldFlag::Spin));
    mCollisionDataWidget->setData(mDataPtr->collisionData(), mFieldFlag.isSet(Ptcl::FieldFlag::Collision));
}


// ========================================================================== //


} // namespace PtclEditor
