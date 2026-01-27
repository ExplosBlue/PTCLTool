#include "editor/components/collapsibleWidget.h"
#include "editor/childEditor/childEditorWidget.h"
#include "editor/childEditor/basicPropertiesWidget.h"
#include "editor/childEditor/emissionPropertiesWidget.h"
#include "editor/childEditor/velocityPropertiesWidget.h"

#include <QScrollArea>


namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::ChildEditorWidget(QWidget* parent) :
    QWidget{parent} {

    mBasicProperties = new BasicPropertiesWidget(this);
    mEmissionProperties = new EmissionPropertiesWidget(this);
    mVelocityProperties = new VelocityPropertiesWidget(this);

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

void ChildEditorWidget::setupLayout(QVBoxLayout* mainLayout) {
    // ChildData row
    auto* childDataRow = new QHBoxLayout();
    auto* childDataLabel = new QLabel("Child Emitter:");
    mEnabledCheckbox.setText("Enabled");

    childDataRow->addWidget(childDataLabel);
    childDataRow->addWidget(&mEnabledCheckbox);
    childDataRow->addStretch();

    mainLayout->addLayout(childDataRow);

    // Section Containers
    mSectionsContainer = new QWidget(this);
    auto* sectionsLayout = new QVBoxLayout(mSectionsContainer);
    sectionsLayout->setContentsMargins(0, 0, 0, 0);

    auto addSection = [sectionsLayout](const QString& title, QWidget* widget) {
        auto* section = new CollapsibleWidget(title);
        section->setContent(widget);
        sectionsLayout->addWidget(section);
    };

    addSection("Basic properties", mBasicProperties);
    addSection("Emission properties", mEmissionProperties);
    addSection("Velocity properties", mVelocityProperties);

    sectionsLayout->addStretch();

    mainLayout->addWidget(mSectionsContainer);
    mainLayout->addStretch();
}

void ChildEditorWidget::setupConnections() {
    // Enabled
    connect(&mEnabledCheckbox, &QCheckBox::clicked, this, [this](bool checked) {
        if (!mDataPtr) { return; }

        mSectionsContainer->setEnabled(checked);
        checked ? mChildFlag.set(Ptcl::ChildFlag::Enabled) : mChildFlag.clear(Ptcl::ChildFlag::Enabled);
        emit flagsUpdated(mChildFlag);
    });

    // Basic Properties
    connect(mBasicProperties, &BasicPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::BasicProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setBasicProperties(properties);
    });

    // Emission Properties
    connect(mEmissionProperties, &EmissionPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::EmissionProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setEmissionProperties(properties);
    });

    // Velocity Properties
    connect(mVelocityProperties, &VelocityPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::VelocityProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setVelocityProperties(properties);
    });

    connect(mVelocityProperties, &VelocityPropertiesWidget::inheritVelUpdated, this, [this](bool inherit) {
        if (!mDataPtr) { return; }
        inherit ? mChildFlag.set(Ptcl::ChildFlag::VelInherit) : mChildFlag.clear(Ptcl::ChildFlag::VelInherit);
        emit flagsUpdated(mChildFlag);
    });
}

void ChildEditorWidget::setChildData(Ptcl::ChildData* childData, const BitFlag<Ptcl::ChildFlag>& childFlag) {
    QSignalBlocker b1(mBasicProperties);

    mDataPtr = childData;
    mChildFlag = childFlag;

    mBasicProperties->setProperties(mDataPtr->basicProperties());
    mEmissionProperties->setProperties(mDataPtr->emissionProperties());


    mVelocityProperties->setProperties(mDataPtr->velocityProperties(), mChildFlag.isSet(Ptcl::ChildFlag::VelInherit));

    const bool isEnabled = mChildFlag.isSet(Ptcl::ChildFlag::Enabled);
    mEnabledCheckbox.setChecked(isEnabled);
    mSectionsContainer->setEnabled(isEnabled);

    setEnabled(true);
}

void ChildEditorWidget::clear() {
    setEnabled(false);
    mDataPtr = nullptr;
}


// ========================================================================== //


} // namespace PtclEditor
