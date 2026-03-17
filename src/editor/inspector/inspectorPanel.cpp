
#include "editor/inspector/inspectorPanel.h"

#include "editor/inspector/alphaPropertiesWidget.h"
#include "editor/inspector/basicPropertiesWidget.h"
#include "editor/inspector/colorPropertiesWidget.h"
#include "editor/inspector/combinerPropertiesWidget.h"
#include "editor/inspector/emissionPropertiesWidget.h"
#include "editor/inspector/fluctuationEditorWidget.h"
#include "editor/inspector/gravityPropertiesWidget.h"
#include "editor/inspector/lifespanPropertiesWidget.h"
#include "editor/inspector/rotationPropertiesWidget.h"
#include "editor/inspector/scalePropertiesWidget.h"
#include "editor/inspector/stripeEditorWidget.h"
#include "editor/inspector/terminationPropertiesWidget.h"
#include "editor/inspector/texturePropertiesWidget.h"
#include "editor/inspector/transformPropertiesWidget.h"
#include "editor/inspector/velocityPropertiesWidget.h"
#include "editor/inspector/volumePropertiesWidget.h"

#include <QScrollArea>

namespace PtclEditor {


// ========================================================================== //


InspectorPanel::InspectorPanel(QWidget* parent) :
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

    mChildEditorWidget = new ChildEditorWidget(this);
    mFluctuationEditorWidget = new FluctuationEditorWidget(this);
    mFieldEditorWidget = new FieldEditorWidget(this);

    mTabWidget = new QTabWidget(this);
    mTabWidget->setTabPosition(QTabWidget::West);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(mTabWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setupConnections();
}

void InspectorPanel::rebuildTabs() {
    if (!mSelection) {
        return;
    }

    mTabWidget->clear();

    switch (mSelection->type()) {
    case Ptcl::Selection::Type::Emitter:
        buildEmitterTabs();
        break;
    case Ptcl::Selection::Type::EmitterChild:
        buildChildTabs();
        break;
    case Ptcl::Selection::Type::EmitterFlux:
        buildFluxTabs();
        break;
    case Ptcl::Selection::Type::EmitterField:
        buildFieldTabs();
        break;
    default:
        break;
    }
}

QWidget* InspectorPanel::wrapInScroll(QWidget* widget) {
    auto* container = new QWidget;
    auto* layout = new QVBoxLayout(container);
    layout->addWidget(widget);
    layout->addStretch();

    auto* scroll = new QScrollArea;
    scroll->setWidget(container);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    return scroll;
}

void InspectorPanel::buildEmitterTabs() {
    mTabWidget->addTab(wrapInScroll(mBasicProperties), "General");

    if (mEmitter->hasStripeData()) {
        mTabWidget->addTab(wrapInScroll(mStripeEditorWidget), "Stripe");
    }

    mTabWidget->addTab(wrapInScroll(mLifespanProperties), "Life");
    mTabWidget->addTab(wrapInScroll(mTerminationProperties), "Termination");
    mTabWidget->addTab(wrapInScroll(mGravityProperties), "Gravity");
    mTabWidget->addTab(wrapInScroll(mEmissionProperties), "Emission");
    mTabWidget->addTab(wrapInScroll(mVolumeProperties), "Volume");
    mTabWidget->addTab(wrapInScroll(mVelocityProperties), "Velocity");
    mTabWidget->addTab(wrapInScroll(mTextureProperties), "Texture");
    mTabWidget->addTab(wrapInScroll(mColorProperties), "Color");
    mTabWidget->addTab(wrapInScroll(mCombinerProperties), "Combiner");
    mTabWidget->addTab(wrapInScroll(mAlphaProperties), "Alpha");
    mTabWidget->addTab(wrapInScroll(mTransformProperties), "Transform");
    mTabWidget->addTab(wrapInScroll(mRotationProperties), "Rotation");
    mTabWidget->addTab(wrapInScroll(mScaleProperties), "Scale");
}

void InspectorPanel::buildChildTabs() {
    mTabWidget->addTab(wrapInScroll(mChildEditorWidget), "Child");
    // TODO
}

void InspectorPanel::buildFluxTabs() {
    mTabWidget->addTab(wrapInScroll(mFluctuationEditorWidget), "Fluctuation");
}

void InspectorPanel::buildFieldTabs() {
    mTabWidget->addTab(wrapInScroll(mFieldEditorWidget), "Field");
    // TODO
}

void InspectorPanel::setupConnections() {
    // Child Editor Widget
    connect(mChildEditorWidget, &ChildEditorWidget::flagsUpdated, this, [this](const BitFlag<Ptcl::ChildFlag>& childFlags) {
        if (!mEmitter) { return; }
        mEmitter->setChildFlags(childFlags);
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
}

void InspectorPanel::setDocument(Ptcl::Document* document) {
    if (mDocument) {
        mDocument->disconnect(this);
    }

    mDocument = document;

    mBasicProperties->setDocument(document);
    mGravityProperties->setDocument(document);
    mTransformProperties->setDocument(document);
    mLifespanProperties->setDocument(document);
    mScaleProperties->setDocument(document);
    mTerminationProperties->setDocument(document);
    mEmissionProperties->setDocument(document);
    mVelocityProperties->setDocument(document);
    mVolumeProperties->setDocument(document);
    mRotationProperties->setDocument(document);
    mAlphaProperties->setDocument(document);
    mCombinerProperties->setDocument(document);
    mColorProperties->setDocument(document);
    mTextureProperties->setDocument(document);
    mFluctuationEditorWidget->setDocument(document);
    mStripeEditorWidget->setDocument(document);

    if (mDocument) {
        connect(mDocument, &Ptcl::Document::emitterChanged, this, [this](s32 setIndex, s32 emitterIndex) {
            if (!mEmitter) {
                return;
            }

            if (setIndex != mSelection->emitterSetIndex() || emitterIndex != mSelection->emitterIndex()) {
                return;
            }

            populateProperties();
            rebuildTabs();
        });
    }
}

void InspectorPanel::setSelection(Ptcl::Selection* selection) {
    if (mSelection) {
        mSelection->disconnect(this);
    }

    mSelection = selection;

    mBasicProperties->setSelection(selection);
    mGravityProperties->setSelection(selection);
    mTransformProperties->setSelection(selection);
    mLifespanProperties->setSelection(selection);
    mScaleProperties->setSelection(selection);
    mTerminationProperties->setSelection(selection);
    mEmissionProperties->setSelection(selection);
    mVelocityProperties->setSelection(selection);
    mVolumeProperties->setSelection(selection);
    mRotationProperties->setSelection(selection);
    mAlphaProperties->setSelection(selection);
    mCombinerProperties->setSelection(selection);
    mColorProperties->setSelection(selection);
    mTextureProperties->setSelection(selection);
    mFluctuationEditorWidget->setSelection(selection);
    mStripeEditorWidget->setSelection(selection);

    if (mSelection) {
        connect(selection, &Ptcl::Selection::selectionChanged, this, [this](s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type type) {
            if (!mDocument) {
                mEmitter = nullptr;
                setEnabled(false);
                return;
            }

            mEmitter = mDocument->emitter(setIndex, emitterIndex);

            rebuildTabs();

            // TODO: Have child widgets handle this themselves
            mChildEditorWidget->setTextureList(mTextureList);

            setEnabled(true);
            populateProperties();
        });
    }
}

void InspectorPanel::populateProperties() {
    QSignalBlocker b15(mChildEditorWidget);
    QSignalBlocker b17(mFieldEditorWidget);
    QSignalBlocker b18(mStripeEditorWidget);

    mChildEditorWidget->setChildData(&mEmitter->childData(), mEmitter->complexProperties().childFlags);
    mChildEditorWidget->setParentColor0(mEmitter->primaryColor());

    mFieldEditorWidget->setData(&mEmitter->fieldData(), mEmitter->complexProperties().fieldFlags);
}


// ========================================================================== //


} // namespace PtclEditor
