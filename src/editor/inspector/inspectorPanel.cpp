
#include "editor/inspector/inspectorPanel.h"

#include "editor/inspector/alphaAnimInspector.h"
#include "editor/inspector/generalEmitterInspector.h"
#include "editor/inspector/colorInspector.h"
#include "editor/inspector/combinerInspector.h"
#include "editor/inspector/emissionInspector.h"
#include "editor/inspector/fluctuationInspector.h"
#include "editor/inspector/gravityInspector.h"
#include "editor/inspector/lifespanInspector.h"
#include "editor/inspector/rotationInspector.h"
#include "editor/inspector/scaleAnimInspector.h"
#include "editor/inspector/stripeInspector.h"
#include "editor/inspector/terminationInspector.h"
#include "editor/inspector/textureInspector.h"
#include "editor/inspector/transformInspector.h"
#include "editor/inspector/velocityInspector.h"
#include "editor/inspector/volumeInspector.h"

#include <QScrollArea>

namespace PtclEditor {


// ========================================================================== //


InspectorPanel::InspectorPanel(QWidget* parent) :
    QWidget{parent} {

    mGeneralInspector = new GeneralEmitterInspector(this);
    mGravityInspector = new GravityInspector(this);
    mTransformInspector = new TransformInspector(this);
    mLifespanInspector = new LifespanInspector(this);
    mTerminationInspector = new TerminationInspector(this);
    mEmissionInspector = new EmissionInspector(this);
    mVelocityInspector = new VelocityInspector(this);
    mVolumeInspector = new VolumeInspector(this);
    mColorInspector = new ColorInspector(this);
    mAlphaAnimInspector = new AlphaAnimInspector(this);
    mRotationInspector = new RotationInspector(this);
    mScaleAnimInspector = new ScaleAnimInspector(this);
    mTextureInspector = new TextureInspector(this);
    mCombinerInspector = new CombinerInspector(this);
    mStripeInspector = new StripeInspector(this);

    mChildEditorWidget = new ChildEditorWidget(this);
    mFluctuationInspector = new FluctuationInspector(this);
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
    mTabWidget->addTab(wrapInScroll(mGeneralInspector), "General");

    if (mEmitter->hasStripeData()) {
        mTabWidget->addTab(wrapInScroll(mStripeInspector), "Stripe");
    }

    mTabWidget->addTab(wrapInScroll(mLifespanInspector), "Life");
    mTabWidget->addTab(wrapInScroll(mTerminationInspector), "Termination");
    mTabWidget->addTab(wrapInScroll(mGravityInspector), "Gravity");
    mTabWidget->addTab(wrapInScroll(mEmissionInspector), "Emission");
    mTabWidget->addTab(wrapInScroll(mVolumeInspector), "Volume");
    mTabWidget->addTab(wrapInScroll(mVelocityInspector), "Velocity");
    mTabWidget->addTab(wrapInScroll(mTextureInspector), "Texture");
    mTabWidget->addTab(wrapInScroll(mColorInspector), "Color");
    mTabWidget->addTab(wrapInScroll(mCombinerInspector), "Combiner");
    mTabWidget->addTab(wrapInScroll(mAlphaAnimInspector), "Alpha");
    mTabWidget->addTab(wrapInScroll(mTransformInspector), "Transform");
    mTabWidget->addTab(wrapInScroll(mRotationInspector), "Rotation");
    mTabWidget->addTab(wrapInScroll(mScaleAnimInspector), "Scale");
}

void InspectorPanel::buildChildTabs() {
    mTabWidget->addTab(wrapInScroll(mChildEditorWidget), "Child");
    // TODO
}

void InspectorPanel::buildFluxTabs() {
    mTabWidget->addTab(wrapInScroll(mFluctuationInspector), "Fluctuation");
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

    mGeneralInspector->setDocument(document);
    mGravityInspector->setDocument(document);
    mTransformInspector->setDocument(document);
    mLifespanInspector->setDocument(document);
    mScaleAnimInspector->setDocument(document);
    mTerminationInspector->setDocument(document);
    mEmissionInspector->setDocument(document);
    mVelocityInspector->setDocument(document);
    mVolumeInspector->setDocument(document);
    mRotationInspector->setDocument(document);
    mAlphaAnimInspector->setDocument(document);
    mCombinerInspector->setDocument(document);
    mColorInspector->setDocument(document);
    mTextureInspector->setDocument(document);
    mFluctuationInspector->setDocument(document);
    mStripeInspector->setDocument(document);

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

    mGeneralInspector->setSelection(selection);
    mGravityInspector->setSelection(selection);
    mTransformInspector->setSelection(selection);
    mLifespanInspector->setSelection(selection);
    mScaleAnimInspector->setSelection(selection);
    mTerminationInspector->setSelection(selection);
    mEmissionInspector->setSelection(selection);
    mVelocityInspector->setSelection(selection);
    mVolumeInspector->setSelection(selection);
    mRotationInspector->setSelection(selection);
    mAlphaAnimInspector->setSelection(selection);
    mCombinerInspector->setSelection(selection);
    mColorInspector->setSelection(selection);
    mTextureInspector->setSelection(selection);
    mFluctuationInspector->setSelection(selection);
    mStripeInspector->setSelection(selection);

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
    QSignalBlocker b18(mStripeInspector);

    mChildEditorWidget->setChildData(&mEmitter->childData(), mEmitter->complexProperties().childFlags);
    mChildEditorWidget->setParentColor0(mEmitter->primaryColor());

    mFieldEditorWidget->setData(&mEmitter->fieldData(), mEmitter->complexProperties().fieldFlags);
}


// ========================================================================== //


} // namespace PtclEditor
