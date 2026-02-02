
#include "editor/emitterWidget/emitterWidget.h"

#include "editor/emitterWidget/alphaPropertiesWidget.h"
#include "editor/emitterWidget/basicPropertiesWidget.h"
#include "editor/emitterWidget/colorPropertiesWidget.h"
#include "editor/emitterWidget/combinerPropertiesWidget.h"
#include "editor/emitterWidget/emissionPropertiesWidget.h"
#include "editor/emitterWidget/gravityPropertiesWidget.h"
#include "editor/emitterWidget/lifespanPropertiesWidget.h"
#include "editor/emitterWidget/rotationPropertiesWidget.h"
#include "editor/emitterWidget/scalePropertiesWidget.h"
#include "editor/emitterWidget/terminationPropertiesWidget.h"
#include "editor/emitterWidget/texturePropertiesWidget.h"
#include "editor/emitterWidget/transformPropertiesWidget.h"
#include "editor/emitterWidget/velocityPropertiesWidget.h"
#include "editor/emitterWidget/volumePropertiesWidget.h"

#include <QScrollArea>

namespace PtclEditor {


// ========================================================================== //


EmitterWidget::EmitterWidget(QWidget* parent) :
    QWidget{parent} {

    mBasicProperties = new BasicPropertiesWidget(this);
    mGravityProperties = new GravityPropertiesWidget(this);
    mTransformProperties = new TransformPropertiesWidget(this);
    mLifespanProperties = new LifespanPropertiesWidget(this);
    mTerminationProperties = new TerminationPropertiesWidget(this);
    mEmissionProperties = new EmissionPropertiesWidget(this);
    mVelocityProperties = new VelocityPropertiesWidget(this);
    mVolumeProperties = new VolumePropertiesWidget(this);
    mColorProperties = new ColorPropertiesWidget(this);
    mAlphaProperties = new AlphaPropertiesWidget(this);
    mRotationProperties = new RotationPropertiesWidget(this);
    mScaleProperties = new ScalePropertiesWidget(this);
    mTextureProperties = new TexturePropertiesWidget(this);
    mCombinerProperties = new CombinerPropertiesWidget(this);
    mStripeEditorWidget = new StripeEditorWidget(this);

    mStackedWidget = new QStackedWidget(this);

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

    mStackedWidget->addWidget(scrollArea);

    // Child editor
    mChildEditorWidget = new ChildEditorWidget(this);
    mStackedWidget->addWidget(mChildEditorWidget);

    // Fluctuation editor
    mFluctuationEditorWidget = new FluctuationEditorWidget(this);
    mStackedWidget->addWidget(mFluctuationEditorWidget);

    // Field editor
    mFieldEditorWidget = new FieldEditorWidget(this);
    mStackedWidget->addWidget(mFieldEditorWidget);

    mStackedWidget->setCurrentIndex(0);

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->addWidget(mStackedWidget);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(outerLayout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setupStandardLayout(mainLayout);
    setupConnections();
}

void EmitterWidget::setupStandardLayout(QVBoxLayout* mainLayout) {
    auto addSection = [mainLayout](const QString& title, QWidget* widget) {
        auto* section = new CollapsibleWidget(title);
        section->setContent(widget);
        mainLayout->addWidget(section);
        return section;
    };

    addSection("Basic properties", mBasicProperties);
    mStripeSection = addSection("Stripe properties", mStripeEditorWidget);
    addSection("Lifespan properties", mLifespanProperties);
    addSection("Termination properties", mTerminationProperties);
    addSection("Gravity properties", mGravityProperties);
    addSection("Emission properties", mEmissionProperties);
    addSection("Volume properties", mVolumeProperties);
    addSection("Velocity properties", mVelocityProperties);
    addSection("Texture properties", mTextureProperties);
    addSection("Color properties", mColorProperties);
    addSection("Combiner properties", mCombinerProperties);
    addSection("Alpha properties", mAlphaProperties);
    addSection("Transform properties", mTransformProperties);
    addSection("Rotation properties", mRotationProperties);
    addSection("Scale properties", mScaleProperties);

    mainLayout->addStretch();
}

void EmitterWidget::setupConnections() {
    // Basic Properties
    connect(mBasicProperties, &BasicPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::BasicProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setBasicProperties(properties);
        updateStripeVisibility();
    });

    connect(mBasicProperties, &BasicPropertiesWidget::emitterTypeChanged, this, [this]() {
        if (!mEmitterPtr) { return; }
        updateStripeVisibility();
        emit emitterTypeChanged();
    });

    connect(mBasicProperties, &BasicPropertiesWidget::emitterNameChanged, this, [this]() {
        if (!mEmitterPtr) { return; }
        emit emitterNameChanged();
    });

    // Texture Properties
    connect(mTextureProperties, &TexturePropertiesWidget::textureUpdated, this, [this](const std::shared_ptr<Ptcl::Texture>& oldTexture, const std::shared_ptr<Ptcl::Texture>& newTexture) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setTexture(newTexture);

        s32 oldIndex = -1;
        s32 newIndex = -1;
        for (size_t i = 0; i < mTextureList->size(); ++i) {
            if ((*mTextureList)[i] == oldTexture) { oldIndex = static_cast<s32>(i); }
            if ((*mTextureList)[i] == newTexture) { newIndex = static_cast<s32>(i); }
        }
        emit textureUpdated(oldIndex, newIndex);
        mCombinerProperties->updateCombinerPreview();
    });

    connect(mTextureProperties, &TexturePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::TextureProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setTextureProperties(properties);
    });

    // Gravity Properties
    connect(mGravityProperties, &GravityPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::GravityProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setGravityProperties(properties);
    });

    // Lifespan Properties
    connect(mLifespanProperties, &LifespanPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::LifespanProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setLifespanProperties(properties);
    });

    // Termination Properties
    connect(mTerminationProperties, &TerminationPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::TerminationProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setTerminationProperties(properties);
    });

    // Emission Properties
    connect(mEmissionProperties, &EmissionPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::EmissionProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setEmissionProperties(properties);
    });

    // Volume Properties
    connect(mVolumeProperties, &VolumePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::VolumeProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setVolumeProperties(properties);
    });

    // Velocity Properties
    connect(mVelocityProperties, &VelocityPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::VelocityProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setVelocityProperties(properties);
    });

    // Transform Properties
    connect(mTransformProperties, &TransformPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::TransformProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setTransformProperties(properties);
    });

    // Color Properties
    connect(mColorProperties, &ColorPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::ColorProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setColorProperties(properties);
        mCombinerProperties->updateCombinerPreview();
        mChildEditorWidget->setParentColor0(mEmitterPtr->colorProperties().colors[0]);
    });

    // Combiner Properties
    connect(mCombinerProperties, &CombinerPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::CombinerProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setCombinerProperties(properties);
    });

    // Alpha Properties
    connect(mAlphaProperties, &AlphaPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::AlphaProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setAlphaProperties(properties);
    });

    // Rotation Properties
    connect(mRotationProperties, &RotationPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::RotationProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setRotationProperties(properties);
    });

    // Scale Properties
    connect(mScaleProperties, &ScalePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::ScaleProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setScaleProperties(properties);
    });

    // Child Editor Widget
    connect(mChildEditorWidget, &ChildEditorWidget::flagsUpdated, this, [this](const BitFlag<Ptcl::ChildFlag>& childFlags) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setChildFlags(childFlags);
        emit complexFlagsChanged();
    });

    connect(mChildEditorWidget, &ChildEditorWidget::textureUpdated, this, [this](const std::shared_ptr<Ptcl::Texture>& oldTexture, const std::shared_ptr<Ptcl::Texture>& newTexture) {
        s32 oldIndex = -1;
        s32 newIndex = -1;
        for (size_t i = 0; i < mTextureList->size(); ++i) {
            if ((*mTextureList)[i] == oldTexture) { oldIndex = static_cast<s32>(i); }
            if ((*mTextureList)[i] == newTexture) { newIndex = static_cast<s32>(i); }
        }
        emit textureUpdated(oldIndex, newIndex);
    });

    // Fluctuation Editor Widget
    connect(mFluctuationEditorWidget, &FluctuationEditorWidget::dataUpdated, this, [this](const Ptcl::FluctuationData& data) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setFluctuationData(data);
    });

    connect(mFluctuationEditorWidget, &FluctuationEditorWidget::flagsUpdated, this, [this](const BitFlag<Ptcl::FluctuationFlag>& fluxFlags) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setFluctuationFlags(fluxFlags);
        complexFlagsChanged();
    });

    // Field Editor Widget
    connect(mFieldEditorWidget, &FieldEditorWidget::flagsUpdated, this, [this](const BitFlag<Ptcl::FieldFlag>& fieldFlags) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setFieldFlags(fieldFlags);
        complexFlagsChanged();
    });

    // Stripe Editor Widget
    connect(mStripeEditorWidget, &StripeEditorWidget::dataUpdated, this, [this](const Ptcl::StripeData& data) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setStripeData(data);
    });

    connect(mStripeEditorWidget, &StripeEditorWidget::flagsUpdated, this, [this](const BitFlag<Ptcl::StripeFlag>& stripeFlags) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setStripeFlags(stripeFlags);
        complexFlagsChanged();
    });

}

void EmitterWidget::setEmitter(Ptcl::Emitter* emitter) {
    QSignalBlocker b1(mBasicProperties);
    QSignalBlocker b2(mGravityProperties);
    QSignalBlocker b3(mTransformProperties);
    QSignalBlocker b4(mLifespanProperties);
    QSignalBlocker b5(mTerminationProperties);
    QSignalBlocker b6(mEmissionProperties);
    QSignalBlocker b7(mVelocityProperties);
    QSignalBlocker b8(mColorProperties);
    QSignalBlocker b9(mAlphaProperties);
    QSignalBlocker b10(mScaleProperties);
    QSignalBlocker b11(mRotationProperties);
    QSignalBlocker b12(mTextureProperties);
    QSignalBlocker b13(mCombinerProperties);
    QSignalBlocker b14(mBasicProperties);
    QSignalBlocker b15(mChildEditorWidget);
    QSignalBlocker b16(mFluctuationEditorWidget);
    QSignalBlocker b17(mFieldEditorWidget);
    QSignalBlocker b18(mStripeEditorWidget);

    mEmitterPtr = emitter;

    mBasicProperties->setProperties(mEmitterPtr->basicProperties());
    mGravityProperties->setProperties(mEmitterPtr->gravityProperties());
    mTransformProperties->setProperties(mEmitterPtr->transformProperties());
    mLifespanProperties->setProperties(mEmitterPtr->lifespanProperties());
    mTerminationProperties->setProperties(mEmitterPtr->terminationProperties());
    mEmissionProperties->setProperties(mEmitterPtr->emissionProperties());
    mVelocityProperties->setProperties(mEmitterPtr->velocityProperties());
    mVolumeProperties->setProperties(mEmitterPtr->volumeProperties());
    mColorProperties->setProperties(mEmitterPtr->colorProperties());
    mAlphaProperties->setProperties(mEmitterPtr->alphaProperties());
    mScaleProperties->setProperties(mEmitterPtr->scaleProperties());
    mRotationProperties->setProperties(mEmitterPtr->rotationProperties());
    mTextureProperties->setProperties(mEmitterPtr->textureProperties(), mEmitterPtr->textureHandle().get());
    mCombinerProperties->setProperties(mEmitterPtr->combinerProperties());
    mCombinerProperties->setCombinerSrc(&mEmitterPtr->textureHandle(), &mEmitterPtr->colorProperties().color0, &mEmitterPtr->colorProperties().colors[0]);

    mChildEditorWidget->setChildData(&mEmitterPtr->childData(), mEmitterPtr->complexProperties().childFlags);
    mChildEditorWidget->setParentColor0(mEmitterPtr->colorProperties().colors[0]);

    mFluctuationEditorWidget->setData(mEmitterPtr->fluctuationData(), mEmitterPtr->complexProperties().fluctuationFlags);
    mFieldEditorWidget->setData(&mEmitterPtr->fieldData(), mEmitterPtr->complexProperties().fieldFlags);
    mStripeEditorWidget->setData(mEmitterPtr->stripeData(), mEmitterPtr->complexProperties().stripeFlags);

    updateStripeVisibility();

    setEnabled(true);
}

void EmitterWidget::setTextureList(const Ptcl::TextureList* textureList) {
    mTextureList = textureList;
    mTextureProperties->setTextureList(textureList);
    mChildEditorWidget->setTextureList(textureList);
}

void EmitterWidget::showStandardEditor() {
    mStackedWidget->setCurrentIndex(0);
}

void EmitterWidget::showChildEditor() {
    if (mStackedWidget) {
        mStackedWidget->setCurrentWidget(mChildEditorWidget);
    }
}

void EmitterWidget::showFluctuationEditor() {
    if (mStackedWidget) {
        mStackedWidget->setCurrentWidget(mFluctuationEditorWidget);
    }
}

void EmitterWidget::showFieldEditor() {
    if (mStackedWidget) {
        mStackedWidget->setCurrentWidget(mFieldEditorWidget);
    }
}

void EmitterWidget::clear() {
    setEnabled(false);
    mEmitterPtr = nullptr;
}

void EmitterWidget::updateStripeVisibility() {
    if (!mEmitterPtr || !mStripeSection) {
        return;
    }

    const auto eType = mEmitterPtr->type();
    const auto bbType = mEmitterPtr->basicProperties().billboardType;

    const bool show = (eType == Ptcl::EmitterType::Complex || eType == Ptcl::EmitterType::Compact) &&
                      (bbType == Ptcl::BillboardType::Stripe || bbType == Ptcl::BillboardType::ComplexStripe);

    mStripeSection->setVisible(show);
}

// ========================================================================== //


} // namespace PtclEditor
