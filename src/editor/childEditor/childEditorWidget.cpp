#include "editor/components/collapsibleWidget.h"
#include "editor/childEditor/childEditorWidget.h"
#include "editor/childEditor/basicPropertiesWidget.h"
#include "editor/childEditor/emissionPropertiesWidget.h"

#include <QScrollArea>


namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::ChildEditorWidget(QWidget* parent) :
    QWidget{parent} {

    mBasicProperties = new BasicPropertiesWidget(this);
    mEmissionProperties = new EmissionPropertiesWidget(this);

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
    auto addSection = [mainLayout](const QString& title, QWidget* widget) {
        auto* section = new CollapsibleWidget(title);
        section->setContent(widget);
        mainLayout->addWidget(section);
    };

    addSection("Basic properties", mBasicProperties);
    addSection("Emission properties", mEmissionProperties);

    mainLayout->addStretch();
}

void ChildEditorWidget::setupConnections() {
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
}

void ChildEditorWidget::setChildData(Ptcl::ChildData* childData) {
    QSignalBlocker b1(mBasicProperties);

    mDataPtr = childData;

    mBasicProperties->setProperties(mDataPtr->basicProperties());
    mEmissionProperties->setProperties(mDataPtr->emissionProperties());

    setEnabled(true);
}

void ChildEditorWidget::clear() {
    setEnabled(false);
    mDataPtr = nullptr;
}


// ========================================================================== //


} // namespace PtclEditor
