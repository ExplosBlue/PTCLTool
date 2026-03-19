#include "editor/inspector/child/childCombinerInspector.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


ChildCombinerInspector::ChildCombinerInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);

    mainLayout->addRow("Blend Function:", &mBlendFuncComboBox);
    mainLayout->addRow("Depth Function:", &mDepthFuncComboBox);
    mainLayout->addRow("Combiner Function:", &mCombinerFuncComboBox);
    mainLayout->addWidget(&mCombinerPreview);

    setupConnections();
}

void ChildCombinerInspector::setupConnections() {
    connect(&mBlendFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto func = mBlendFuncComboBox.currentEnum();
        setEmitterProperty(
            "Set Child Blend Function",
            "SetChildBlendFunc",
            &Ptcl::Emitter::childBlendFunc,
            &Ptcl::Emitter::setChildBlendFunc,
            func
        );
    });

    connect(&mDepthFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto func = mDepthFuncComboBox.currentEnum();
        setEmitterProperty(
            "Set Child Depth Function",
            "SetChildDepthFunc",
            &Ptcl::Emitter::childDepthFunc,
            &Ptcl::Emitter::setChildDepthFunc,
            func
        );
    });

    connect(&mCombinerFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto func = mCombinerFuncComboBox.currentEnum();
        setEmitterProperty(
            "Set Child Combiner Function",
            "SetChildCombinerFunc",
            &Ptcl::Emitter::childCombinerFunc,
            &Ptcl::Emitter::setChildCombinerFunc,
            func
        );
    });
}

void ChildCombinerInspector::populateProperties() {
    QSignalBlocker b1(mBlendFuncComboBox);
    QSignalBlocker b2(mDepthFuncComboBox);
    QSignalBlocker b3(mCombinerFuncComboBox);

    mBlendFuncComboBox.setCurrentEnum(mEmitter->childBlendFunc());
    mDepthFuncComboBox.setCurrentEnum(mEmitter->childDepthFunc());
    mCombinerFuncComboBox.setCurrentEnum(mEmitter->childCombinerFunc());
    mCombinerPreview.setConfig(static_cast<s32>(mEmitter->childCombinerFunc()));

    mCombinerPreview.setCombinerSrc(&mEmitter->childTextureHandle(), &mEmitter->childSecondaryColor(), &mEmitter->childPrimaryColor());
    mCombinerPreview.updateStages();
}


// ========================================================================== //


} // namespace PtclEditor
