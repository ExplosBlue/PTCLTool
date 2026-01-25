#include "editor/collapsibleWidget.h"
#include "editor/emitterWidget.h"

#include <QScrollArea>

namespace PtclEditor {


// ========================================================================== //


EmitterWidget::EmitterWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QVBoxLayout(this);

    auto* containerWidget = new QWidget;
    containerWidget->setLayout(mainLayout);

    containerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    containerWidget->setMinimumWidth(400);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(containerWidget);
    scrollArea->setWidgetResizable(true);

    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->addWidget(scrollArea);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(outerLayout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Basic Properties
    auto basicPropertiesSection = new CollapsibleWidget("Basic properties", this);
    basicPropertiesSection->setContent(&mBasicProperties);
    mainLayout->addWidget(basicPropertiesSection);

    // Lifespan Properties
    auto lifespanPropertiesSection = new CollapsibleWidget("Lifespan properties", this);
    lifespanPropertiesSection->setContent(&mLifespanProperties);
    mainLayout->addWidget(lifespanPropertiesSection);

    // Termination Properties
    auto terminationPropertiesSection = new CollapsibleWidget("Termination properties", this);
    terminationPropertiesSection->setContent(&mTerminationProperties);
    mainLayout->addWidget(terminationPropertiesSection);

    // Gravity Properties
    auto gravityPropertiesSection = new CollapsibleWidget("Gravity properties", this);
    gravityPropertiesSection->setContent(&mGravityProperties);
    mainLayout->addWidget(gravityPropertiesSection);

    // Emission Properties
    auto emissionPropertiesSection = new CollapsibleWidget("Emission properties", this);
    emissionPropertiesSection->setContent(&mEmissionProperties);
    mainLayout->addWidget(emissionPropertiesSection);

    // Volume Properties
    auto volumePropertiesSection = new CollapsibleWidget("Volume properties", this);
    volumePropertiesSection->setContent(&mVolumeProperties);
    mainLayout->addWidget(volumePropertiesSection);

    // Velocity Properties
    auto velocityPropertiesSection = new CollapsibleWidget("Velocity properties", this);
    velocityPropertiesSection->setContent(&mVelocityProperties);
    mainLayout->addWidget(velocityPropertiesSection);

    // Texture Properties
    auto texPropertiesSection = new CollapsibleWidget("Texture properties", this);
    texPropertiesSection->setContent(&mTextureProperties);
    mainLayout->addWidget(texPropertiesSection);

    // Color Properties
    auto colorPropertiesSection = new CollapsibleWidget("Color properties", this);
    colorPropertiesSection->setContent(&mColorProperties);
    mainLayout->addWidget(colorPropertiesSection);

    // Combiner Properties
    auto combinerPropertiesSection = new CollapsibleWidget("Combiner properties", this);
    combinerPropertiesSection->setContent(&mCombinerProperties);
    mainLayout->addWidget(combinerPropertiesSection);

    // Alpha Properties
    auto alphaPropertiesSection = new CollapsibleWidget("Alpha properties", this);
    alphaPropertiesSection->setContent(&mAlphaProperties);
    mainLayout->addWidget(alphaPropertiesSection);

    // Transform Properties
    auto transformPropertiesSection = new CollapsibleWidget("Transform properties", this);
    transformPropertiesSection->setContent(&mTransformProperties);
    mainLayout->addWidget(transformPropertiesSection);

    // Rotation Properties
    auto rotationPropertiesSection = new CollapsibleWidget("Rotation properties", this);
    rotationPropertiesSection->setContent(&mRotationProperties);
    mainLayout->addWidget(rotationPropertiesSection);

    // Scale Properties
    auto scalePropertiesSection = new CollapsibleWidget("Scale properties", this);
    scalePropertiesSection->setContent(&mScaleProperties);
    mainLayout->addWidget(scalePropertiesSection);

    setupConnections();
}

void EmitterWidget::setupConnections() {
    // Basic Properties
    connect(&mBasicProperties, &BasicPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::BasicProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setBasicProperties(properties);
    });

    // Texture Properties
    connect(&mTextureProperties, &TexturePropertiesWidget::textureUpdated, this, [this](const std::shared_ptr<Ptcl::Texture>& oldTexture, const std::shared_ptr<Ptcl::Texture>& newTexture) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setTexture(newTexture);

        s32 oldIndex = -1;
        s32 newIndex = -1;
        for (size_t i = 0; i < mTextureList->size(); ++i) {
            if ((*mTextureList)[i] == oldTexture) { oldIndex = static_cast<s32>(i); }
            if ((*mTextureList)[i] == newTexture) { newIndex = static_cast<s32>(i); }
        }
        emit textureUpdated(oldIndex, newIndex);
        mCombinerProperties.updateCombinerPreview();
    });

    connect(&mTextureProperties, &TexturePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::TextureProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setTextureProperties(properties);
    });

    // Gravity Properties
    connect(&mGravityProperties, &GravityPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::GravityProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setGravityProperties(properties);
    });

    // Lifespan Properties
    connect(&mLifespanProperties, &LifespanPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::LifespanProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setLifespanProperties(properties);
    });

    // Termination Properties
    connect(&mTerminationProperties, &TerminationPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::TerminationProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setTerminationProperties(properties);
    });

    // Emission Properties
    connect(&mEmissionProperties, &EmissionPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::EmissionProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setEmissionProperties(properties);
    });

    // Volume Properties
    connect(&mVolumeProperties, &VolumePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::VolumeProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setVolumeProperties(properties);
    });

    // Velocity Properties
    connect(&mVelocityProperties, &VelocityPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::VelocityProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setVelocityProperties(properties);
    });

    // Transform Properties
    connect(&mTransformProperties, &TransformPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::TransformProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setTransformProperties(properties);
    });

    // Color Properties
    connect(&mColorProperties, &ColorPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::ColorProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setColorProperties(properties);
        mCombinerProperties.updateCombinerPreview();
    });

    // Combiner Properties
    connect(&mCombinerProperties, &CombinerPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::CombinerProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setCombinerProperties(properties);
    });

    // Alpha Properties
    connect(&mAlphaProperties, &AlphaPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::AlphaProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setAlphaProperties(properties);
    });

    // Rotation Properties
    connect(&mRotationProperties, &RotationPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::RotationProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setRotationProperties(properties);
    });

    // Scale Properties
    connect(&mScaleProperties, &ScalePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::ScaleProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setScaleProperties(properties);
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

    mEmitterPtr = emitter;

    mBasicProperties.setProperties(mEmitterPtr->basicProperties());
    mGravityProperties.setProperties(mEmitterPtr->gravityProperties());
    mTransformProperties.setProperties(mEmitterPtr->transformProperties());
    mLifespanProperties.setProperties(mEmitterPtr->lifespanProperties());
    mTerminationProperties.setProperties(mEmitterPtr->terminationProperties());
    mEmissionProperties.setProperties(mEmitterPtr->emissionProperties());
    mVelocityProperties.setProperties(mEmitterPtr->velocityProperties());
    mVolumeProperties.setProperties(mEmitterPtr->volumeProperties());
    mColorProperties.setProperties(mEmitterPtr->colorProperties());
    mAlphaProperties.setProperties(mEmitterPtr->alphaProperties());
    mScaleProperties.setProperties(mEmitterPtr->scaleProperties());
    mRotationProperties.setProperties(mEmitterPtr->rotationProperties());
    mTextureProperties.setProperties(mEmitterPtr->textureProperties(), mEmitterPtr->textureHandle().get());
    mCombinerProperties.setProperties(mEmitterPtr->combinerProperties());
    mCombinerProperties.setCombinerSrc(&mEmitterPtr->textureHandle(), &mEmitterPtr->colorProperties().color0, &mEmitterPtr->colorProperties().colors[0]);

    setEnabled(true);
}

void EmitterWidget::setTextureList(const Ptcl::TextureList* textureList) {
    mTextureList = textureList;
    mTextureProperties.setTextureList(textureList);
}

void EmitterWidget::clear() {
    setEnabled(false);
    mEmitterPtr = nullptr;
}


// ========================================================================== //


} // namespace PtclEditor
