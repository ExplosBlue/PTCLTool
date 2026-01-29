#include "editor/childEditor/combinerPropertiesWidget.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::CombinerPropertiesWidget::CombinerPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Blend Function:", &mBlendFuncComboBox);
    mainLayout->addRow("Depth Function:", &mDepthFuncComboBox);
    mainLayout->addRow("Combiner Function:", &mCombinerFuncComboBox);
    mainLayout->addWidget(&mCombinerPreview);

    connect(&mBlendFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.blendFunc = mBlendFuncComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    connect(&mDepthFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.depthFunc = mDepthFuncComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    connect(&mCombinerFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        auto value = mCombinerFuncComboBox.currentEnum();
        mProps.combinerFunc = value;
        mCombinerPreview.setConfig(static_cast<s32>(value));
        emit propertiesUpdated(mProps);
    });
}

void ChildEditorWidget::CombinerPropertiesWidget::setProperties(const Ptcl::ChildData::CombinerProperties& properties) {
    QSignalBlocker b1(mBlendFuncComboBox);
    QSignalBlocker b2(mDepthFuncComboBox);
    QSignalBlocker b3(mCombinerFuncComboBox);

    mProps = properties;

    mBlendFuncComboBox.setCurrentEnum(mProps.blendFunc);
    mDepthFuncComboBox.setCurrentEnum(mProps.depthFunc);
    mCombinerFuncComboBox.setCurrentEnum(mProps.combinerFunc);
    mCombinerPreview.setConfig(static_cast<s32>(mProps.combinerFunc));
}

void ChildEditorWidget::CombinerPropertiesWidget::setCombinerSrc(const Ptcl::TextureHandle* texture, const Ptcl::binColor3f* constant, const Ptcl::binColor4f* primary) {
    mCombinerPreview.setCombinerSrc(texture, constant, primary);
}

void ChildEditorWidget::CombinerPropertiesWidget::updateCombinerPreview() {
    mCombinerPreview.updateStages();
}


// ========================================================================== //


} // namespace PtclEditor
