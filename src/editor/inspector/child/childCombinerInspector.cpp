#include "editor/inspector/child/childCombinerInspector.h"

#include <array>

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


static const std::array blendFuncOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::BlendFuncType>{ Ptcl::BlendFuncType::Translucent,  "Translucent",  "Standard alpha blending (SrcAlpha, OneMinusSrcAlpha)." },
    EnumOption<Ptcl::BlendFuncType>{ Ptcl::BlendFuncType::Additive,     "Additive",     "Additive blending (SrcAlpha, One). Creates glow and bright effects." },
    EnumOption<Ptcl::BlendFuncType>{ Ptcl::BlendFuncType::Subtractive,  "Subtractive",  "Subtractive blending. Creates dark and shadow effects." },
    EnumOption<Ptcl::BlendFuncType>{ Ptcl::BlendFuncType::Transparence, "Transparence", "Alternative transparency blending mode." },
};

static const std::array depthFuncOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::DepthFuncType>{ Ptcl::DepthFuncType::Unk0, "Standard",    "Depth test enabled, depth write enabled." },
    EnumOption<Ptcl::DepthFuncType>{ Ptcl::DepthFuncType::Unk1, "No Test",     "Depth test disabled, depth write enabled." },
    EnumOption<Ptcl::DepthFuncType>{ Ptcl::DepthFuncType::Unk2, "No Write",    "Depth test enabled, depth write disabled." },
};

static const std::array combinerFuncOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig0,  "Combiner Config 0",  "Color combiner configuration 0." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig1,  "Combiner Config 1",  "Color combiner configuration 1." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig2,  "Combiner Config 2",  "Color combiner configuration 2." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig3,  "Combiner Config 3",  "Color combiner configuration 3." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig4,  "Combiner Config 4",  "Color combiner configuration 4." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig5,  "Combiner Config 5",  "Color combiner configuration 5." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig6,  "Combiner Config 6",  "Color combiner configuration 6." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig7,  "Combiner Config 7",  "Color combiner configuration 7." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig8,  "Combiner Config 8",  "Color combiner configuration 8." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig9,  "Combiner Config 9",  "Color combiner configuration 9." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig10, "Combiner Config 10", "Color combiner configuration 10." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig11, "Combiner Config 11", "Color combiner configuration 11." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig12, "Combiner Config 12", "Color combiner configuration 12." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig13, "Combiner Config 13", "Color combiner configuration 13." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig14, "Combiner Config 14", "Color combiner configuration 14." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig15, "Combiner Config 15", "Color combiner configuration 15." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig16, "Combiner Config 16", "Color combiner configuration 16." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig17, "Combiner Config 17", "Color combiner configuration 17." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig18, "Combiner Config 18", "Color combiner configuration 18." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig19, "Combiner Config 19", "Color combiner configuration 19." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig20, "Combiner Config 20", "Color combiner configuration 20." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig21, "Combiner Config 21", "Color combiner configuration 21." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig22, "Combiner Config 22", "Color combiner configuration 22." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig23, "Combiner Config 23", "Color combiner configuration 23." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig24, "Combiner Config 24", "Color combiner configuration 24." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig25, "Combiner Config 25", "Color combiner configuration 25." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig26, "Combiner Config 26", "Color combiner configuration 26." },
    EnumOption<Ptcl::ColorCombinerFuncType>{ Ptcl::ColorCombinerFuncType::CombinerConfig27, "Combiner Config 27", "Color combiner configuration 27." },
};


ChildCombinerInspector::ChildCombinerInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    mBlendFuncComboBox.setOptions(blendFuncOptions);
    mainLayout->addRow("Blend Function:", &mBlendFuncComboBox);
    mDepthFuncComboBox.setOptions(depthFuncOptions);
    mainLayout->addRow("Depth Function:", &mDepthFuncComboBox);
    mCombinerFuncComboBox.setOptions(combinerFuncOptions);
    mCombinerFuncComboBox.setToolTip("Selects a GPU combiner configuration defining how texture, primary, and constant colors are combined.");
    mainLayout->addRow("Color Combiner:", &mCombinerFuncComboBox);
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

    mCombinerPreview.setCombinerSrc(&mEmitter->childTextureHandle(), &mEmitter->childPrimaryColor(), &mEmitter->childSecondaryColor());
    mCombinerPreview.updateStages();
}


// ========================================================================== //


} // namespace PtclEditor
