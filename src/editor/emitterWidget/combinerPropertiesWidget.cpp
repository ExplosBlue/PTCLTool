#include "editor/emitterWidget/combinerPropertiesWidget.h"

#include <QFormLayout>

namespace PtclEditor {


// ========================================================================== //


CombinerPropertiesWidget::CombinerPropertiesWidget(QWidget* parent) :
    EmitterWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);

    mFogCheckBox.setText("Enable Fog");

    mainLayout->addRow("Fog:", &mFogCheckBox);
    mainLayout->addRow("Blend Function:", &mBlendFuncComboBox);
    mainLayout->addRow("Depth Function:", &mDepthFuncComboBox);
    mainLayout->addRow("Combiner Function:", &mCombinerFuncComboBox);
    mainLayout->addWidget(&mCombinerPreview);

    setupConnections();
}

void CombinerPropertiesWidget::setupConnections() {
    connect(&mFogCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Set Fog Enabled",
            "SetFogEnabled",
            &Ptcl::Emitter::isFogEnabled,
            &Ptcl::Emitter::setIsFogEnabled,
            checked
        );
    });

    connect(&mBlendFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto func = mBlendFuncComboBox.currentEnum();
        setEmitterProperty(
            "Set Blend Function",
            "SetBlendFunction",
            &Ptcl::Emitter::blendFunction,
            &Ptcl::Emitter::setBlendFunction,
            func
        );
    });

    connect(&mDepthFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto func = mDepthFuncComboBox.currentEnum();
        setEmitterProperty(
            "Set Depth Function",
            "SetDepthFunction",
            &Ptcl::Emitter::depthFunction,
            &Ptcl::Emitter::setDepthFunction,
            func
        );
    });

    connect(&mCombinerFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto func = mCombinerFuncComboBox.currentEnum();
        setEmitterProperty(
            "Set Combiner Function",
            "SetCombinerFunction",
            &Ptcl::Emitter::combinerFunction,
            &Ptcl::Emitter::setCombinerFunction,
            func
        );
    });
}

void CombinerPropertiesWidget::populateProperties() {
    QSignalBlocker b1(mBlendFuncComboBox);
    QSignalBlocker b2(mDepthFuncComboBox);
    QSignalBlocker b3(mCombinerFuncComboBox);
    QSignalBlocker b4(mFogCheckBox);

    mFogCheckBox.setChecked(mEmitter->isFogEnabled());
    mBlendFuncComboBox.setCurrentEnum(mEmitter->blendFunction());
    mDepthFuncComboBox.setCurrentEnum(mEmitter->depthFunction());
    mCombinerFuncComboBox.setCurrentEnum(mEmitter->combinerFunction());
    mCombinerPreview.setConfig(static_cast<s32>(mEmitter->combinerFunction()));
    mCombinerPreview.setCombinerSrc(&mEmitter->textureHandle(), &mEmitter->colorProperties().color1, &mEmitter->colorProperties().color0[0]);
}

void CombinerPropertiesWidget::updateCombinerPreview() {
    mCombinerPreview.updateStages();
}


// ========================================================================== //


} // namespace PtclEditor
