#include "editor/components/collapsibleWidget.h"
#include "editor/fieldEditor/fieldEditorWidget.h"
#include "editor/fieldEditor/randomDataWidget.h"

#include <QScrollArea>


namespace PtclEditor {


// ========================================================================== //


FieldEditorWidget::FieldEditorWidget(QWidget* parent) :
    QWidget{parent} {

    mRandomDataWidget = new RandomDataWidget(this);

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
}

void FieldEditorWidget::setData(Ptcl::FieldData* fieldData, const BitFlag<Ptcl::FieldFlag>& fieldFlag) {
    QSignalBlocker b1(mRandomDataWidget);

    mDataPtr = fieldData;
    mFieldFlag = fieldFlag;

    mRandomDataWidget->setData(mDataPtr->randomData(), mFieldFlag.isSet(Ptcl::FieldFlag::Random));
}


// ========================================================================== //


} // namespace PtclEditor
