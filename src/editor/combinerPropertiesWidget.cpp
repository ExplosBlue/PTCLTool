#include "editor/combinerPropertiesWidget.h"

#include <QFormLayout>


// ========================================================================== //


CombinerPropertiesWidget::CombinerPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Blend Function", &mBlendFuncComboBox);
    mainLayout->addRow("Depth Function:", &mDepthFuncComboBox);
    mainLayout->addRow("Combiner Function:", &mCombinerFuncComboBox);

    connect(&mBlendFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.blendFunc = mBlendFuncComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    connect(&mDepthFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.depthFunc = mDepthFuncComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    connect(&mCombinerFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.combinerFunc = mCombinerFuncComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });
}

void CombinerPropertiesWidget::setProperties(const Ptcl::CombinerProperties& properties) {
    QSignalBlocker b1(mBlendFuncComboBox);
    QSignalBlocker b2(mDepthFuncComboBox);
    QSignalBlocker b3(mCombinerFuncComboBox);

    mProps = properties;

    mBlendFuncComboBox.setCurrentEnum(mProps.blendFunc);
    mDepthFuncComboBox.setCurrentEnum(mProps.depthFunc);
    mCombinerFuncComboBox.setCurrentEnum(mProps.combinerFunc);
}


// ========================================================================== //
