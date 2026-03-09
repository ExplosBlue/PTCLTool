
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
    scrollArea->setFrameShape(QFrame::NoFrame);

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
    // Texture Properties
    connect(mTextureProperties, &TexturePropertiesWidget::textureUpdated, this, [this](const std::shared_ptr<Ptcl::Texture>& oldTexture, const std::shared_ptr<Ptcl::Texture>& newTexture) {
        if (!mEmitter) { return; }
        mEmitter->setTexture(newTexture);
        mCombinerProperties->updateCombinerPreview();
        emit propertiesChanged();
    });

    connect(mTextureProperties, &TexturePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::TextureProperties& properties) {
        if (!mEmitter) { return; }
        mEmitter->setTextureProperties(properties);
        emit propertiesChanged();
    });

    // Lifespan Properties
    connect(mLifespanProperties, &LifespanPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::LifespanProperties& properties) {
        if (!mEmitter) { return; }
        mEmitter->setLifespanProperties(properties);
        emit propertiesChanged();
    });

    // Termination Properties
    connect(mTerminationProperties, &TerminationPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::TerminationProperties& properties) {
        if (!mEmitter) { return; }
        mEmitter->setTerminationProperties(properties);
        emit propertiesChanged();
    });

    // Emission Properties
    connect(mEmissionProperties, &EmissionPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::EmissionProperties& properties) {
        if (!mEmitter) { return; }
        mEmitter->setEmissionProperties(properties);
        emit propertiesChanged();
    });

    // Volume Properties
    connect(mVolumeProperties, &VolumePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::VolumeProperties& properties) {
        if (!mEmitter) { return; }
        mEmitter->setVolumeProperties(properties);
        emit propertiesChanged();
    });

    // Velocity Properties
    connect(mVelocityProperties, &VelocityPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::VelocityProperties& properties) {
        if (!mEmitter) { return; }
        mEmitter->setVelocityProperties(properties);
        emit propertiesChanged();
    });

    // Color Properties
    connect(mColorProperties, &ColorPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::ColorProperties& properties) {
        if (!mEmitter) { return; }
        mEmitter->setColorProperties(properties);
        mCombinerProperties->updateCombinerPreview();
        mChildEditorWidget->setParentColor0(mEmitter->colorProperties().color0[0]);
        emit propertiesChanged();
    });

    // Combiner Properties
    connect(mCombinerProperties, &CombinerPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::CombinerProperties& properties) {
        if (!mEmitter) { return; }
        mEmitter->setCombinerProperties(properties);
        emit propertiesChanged();
    });

    // Alpha Properties
    connect(mAlphaProperties, &AlphaPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::AlphaProperties& properties) {
        if (!mEmitter) { return; }
        mEmitter->setAlphaProperties(properties);
        emit propertiesChanged();
    });

    // Rotation Properties
    connect(mRotationProperties, &RotationPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::RotationProperties& properties) {
        if (!mEmitter) { return; }
        mEmitter->setRotationProperties(properties);
        emit propertiesChanged();
    });

    // Scale Properties
    connect(mScaleProperties, &ScalePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::Emitter::ScaleProperties& properties) {
        if (!mEmitter) { return; }
        mEmitter->setScaleProperties(properties);
        emit propertiesChanged();
    });

    // Child Editor Widget
    connect(mChildEditorWidget, &ChildEditorWidget::flagsUpdated, this, [this](const BitFlag<Ptcl::ChildFlag>& childFlags) {
        if (!mEmitter) { return; }
        mEmitter->setChildFlags(childFlags);
        emit complexFlagsChanged();
        emit propertiesChanged();
    });

    // Fluctuation Editor Widget
    connect(mFluctuationEditorWidget, &FluctuationEditorWidget::dataUpdated, this, [this](const Ptcl::FluctuationData& data) {
        if (!mEmitter) { return; }
        mEmitter->setFluctuationData(data);
        emit propertiesChanged();
    });

    connect(mFluctuationEditorWidget, &FluctuationEditorWidget::flagsUpdated, this, [this](const BitFlag<Ptcl::FluctuationFlag>& fluxFlags) {
        if (!mEmitter) { return; }
        mEmitter->setFluctuationFlags(fluxFlags);
        emit complexFlagsChanged();
        emit propertiesChanged();
    });

    // Field Editor Widget
    connect(mFieldEditorWidget, &FieldEditorWidget::flagsUpdated, this, [this](const BitFlag<Ptcl::FieldFlag>& fieldFlags) {
        if (!mEmitter) { return; }
        mEmitter->setFieldFlags(fieldFlags);
        emit complexFlagsChanged();
        emit propertiesChanged();
    });

    // Stripe Editor Widget
    connect(mStripeEditorWidget, &StripeEditorWidget::dataUpdated, this, [this](const Ptcl::StripeData& data) {
        if (!mEmitter) { return; }
        mEmitter->setStripeData(data);
        emit propertiesChanged();
    });

    connect(mStripeEditorWidget, &StripeEditorWidget::flagsUpdated, this, [this](const BitFlag<Ptcl::StripeFlag>& stripeFlags) {
        if (!mEmitter) { return; }
        mEmitter->setStripeFlags(stripeFlags);
        emit complexFlagsChanged();
        emit propertiesChanged();
    });

}

void EmitterWidget::setDocument(Ptcl::Document* document) {
    mDocument = document;

    mBasicProperties->setDocument(document);
    mGravityProperties->setDocument(document);
    mTransformProperties->setDocument(document);
}

void EmitterWidget::setSelection(Ptcl::Selection* selection) {
    mSelection = selection;

    mBasicProperties->setSelection(selection);
    mGravityProperties->setSelection(selection);
    mTransformProperties->setSelection(selection);

    connect(selection, &Ptcl::Selection::selectionChanged, this, [this](s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type type) {
        if (!mDocument) {
            mEmitter = nullptr;
            setEnabled(false);
            return;
        }

        mEmitter = mDocument->emitter(setIndex, emitterIndex);

        switch (type) {
        case Ptcl::Selection::Type::Emitter:
            mStackedWidget->setCurrentIndex(0);
            break;
        case Ptcl::Selection::Type::EmitterChild:
            mStackedWidget->setCurrentWidget(mChildEditorWidget);
            break;
        case Ptcl::Selection::Type::EmitterFlux:
            mStackedWidget->setCurrentWidget(mFluctuationEditorWidget);
            break;
        case Ptcl::Selection::Type::EmitterField:
            mStackedWidget->setCurrentWidget(mFieldEditorWidget);
            break;
        default:
            break;
        }

        // TODO: Have child widgets handle this themselves
        mTextureList = &mDocument->textures();
        mTextureProperties->setTextureList(mTextureList);
        mChildEditorWidget->setTextureList(mTextureList);

        setEnabled(true);
        populateProperties();
    });
}

void EmitterWidget::populateProperties() {
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
    QSignalBlocker b15(mChildEditorWidget);
    QSignalBlocker b16(mFluctuationEditorWidget);
    QSignalBlocker b17(mFieldEditorWidget);
    QSignalBlocker b18(mStripeEditorWidget);

    mLifespanProperties->setProperties(mEmitter->lifespanProperties());
    mTerminationProperties->setProperties(mEmitter->terminationProperties());
    mEmissionProperties->setProperties(mEmitter->emissionProperties());
    mVelocityProperties->setProperties(mEmitter->velocityProperties());
    mVolumeProperties->setProperties(mEmitter->volumeProperties());
    mColorProperties->setProperties(mEmitter->colorProperties());
    mAlphaProperties->setProperties(mEmitter->alphaProperties());
    mScaleProperties->setProperties(mEmitter->scaleProperties());
    mRotationProperties->setProperties(mEmitter->rotationProperties());
    mTextureProperties->setProperties(mEmitter->textureProperties(), mEmitter->textureHandle().get());
    mCombinerProperties->setProperties(mEmitter->combinerProperties());
    mCombinerProperties->setCombinerSrc(&mEmitter->textureHandle(), &mEmitter->colorProperties().color1, &mEmitter->colorProperties().color0[0]);

    mChildEditorWidget->setChildData(&mEmitter->childData(), mEmitter->complexProperties().childFlags);
    mChildEditorWidget->setParentColor0(mEmitter->colorProperties().color0[0]);

    mFluctuationEditorWidget->setData(mEmitter->fluctuationData(), mEmitter->complexProperties().fluctuationFlags);
    mFieldEditorWidget->setData(&mEmitter->fieldData(), mEmitter->complexProperties().fieldFlags);
    mStripeEditorWidget->setData(mEmitter->stripeData(), mEmitter->complexProperties().stripeFlags);

    updateStripeVisibility();
}

void EmitterWidget::updateStripeVisibility() {
    if (!mEmitter || !mStripeSection) {
        return;
    }

    const auto eType = mEmitter->type();
    const auto bbType = mEmitter->billboardType();

    const bool show = (eType == Ptcl::EmitterType::Complex || eType == Ptcl::EmitterType::Compact) &&
                      (bbType == Ptcl::BillboardType::Stripe || bbType == Ptcl::BillboardType::ComplexStripe);

    mStripeSection->setVisible(show);
}

// ========================================================================== //


} // namespace PtclEditor
