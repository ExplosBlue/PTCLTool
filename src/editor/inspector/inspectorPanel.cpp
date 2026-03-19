
#include "editor/inspector/inspectorPanel.h"

#include "editor/inspector/alphaAnimInspector.h"
#include "editor/inspector/generalEmitterInspector.h"
#include "editor/inspector/colorInspector.h"
#include "editor/inspector/combinerInspector.h"
#include "editor/inspector/emissionInspector.h"
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

#include "editor/inspector/fluctuationInspector.h"

#include "editor/inspector/child/childAlphaInspector.h"
#include "editor/inspector/child/childColorInspector.h"
#include "editor/inspector/child/childCombinerInspector.h"
#include "editor/inspector/child/childEmissionInspector.h"
#include "editor/inspector/child/childGeneralInspector.h"
#include "editor/inspector/child/childRotationInspector.h"
#include "editor/inspector/child/childScaleInspector.h"
#include "editor/inspector/child/childTextureInspector.h"
#include "editor/inspector/child/childVelocityInspector.h"

#include "editor/inspector/field/fieldCollisionInspector.h"
#include "editor/inspector/field/fieldConvergenceInspector.h"
#include "editor/inspector/field/fieldMagnetInspector.h"
#include "editor/inspector/field/fieldPosAddInspector.h"
#include "editor/inspector/field/fieldRandomInspector.h"
#include "editor/inspector/field/fieldSpinInspector.h"
#include "util/nameValidator.h"

#include <QScrollArea>

namespace PtclEditor {


// ========================================================================== //


InspectorPanel::InspectorPanel(QWidget* parent) :
    QWidget{parent} {

    mProjNameLineEdit = new QLineEdit(this);
    mProjNameLineEdit->setPlaceholderText("PTCLProject");
    mProjNameLineEdit->setValidator(new EmitterNameValidator(mProjNameLineEdit));

    connect(mProjNameLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        mDocument->setProjectName(text);
    });

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

    mFieldCollisionInspector = new FieldCollisionInspector(this);
    mFieldConvergenceInspector = new FieldConvergenceInspector(this);
    mFieldMagnetInspector = new FieldMagnetInspector(this);
    mFieldPosAddInspector = new FieldPosAddInspector(this);
    mFieldRandomInspector = new FieldRandomInspector(this);
    mFieldSpinInspector = new FieldSpinInspector(this);

    mChildAlphaInspector = new ChildAlphaInspector(this);
    mChildColorInspector = new ChildColorInspector(this);
    mChildCombinerInspector = new ChildCombinerInspector(this);
    mChildEmissionInspector = new ChildEmissionInspector(this);
    mChildGeneralInspector = new ChildGeneralInspector(this);
    mChildRotationInspector = new ChildRotationInspector(this);
    mChildScaleInspector = new ChildScaleInspector(this);
    mChildTextureInspector = new ChildTextureInspector(this);
    mChildVelocityInspector = new ChildVelocityInspector(this);

    mFluctuationInspector = new FluctuationInspector(this);

    // Project Properties
    auto* projectPropertiesLayout = new QHBoxLayout;
    projectPropertiesLayout->addWidget(new QLabel("Project Name"));
    projectPropertiesLayout->addWidget(mProjNameLineEdit);

    mTabStack = new QStackedWidget(this);

    mEmitterTabs = new QTabWidget(this);
    mEmitterTabs->setTabPosition(QTabWidget::West);
    mTabStack->addWidget(mEmitterTabs);

    mChildTabs = new QTabWidget(this);
    mChildTabs->setTabPosition(QTabWidget::West);
    mTabStack->addWidget(mChildTabs);

    mFieldTabs = new QTabWidget(this);
    mFieldTabs->setTabPosition(QTabWidget::West);
    mTabStack->addWidget(mFieldTabs);

    mFluxTabs = new QTabWidget(this);
    mFluxTabs->setTabPosition(QTabWidget::West);
    mTabStack->addWidget(mFluxTabs);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(projectPropertiesLayout);
    layout->addWidget(mTabStack);
    layout->setContentsMargins(0, 0, 0, 0);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    buildTabs();
}

void InspectorPanel::buildTabs() {
    // This is dumb, but fixes some issues with some tabs being incorrectly sized
    // Individual inspector widgets should probably be adjusted instead so this can be removed
    auto wrapInScroll = [this](QWidget* content) {
        auto* scroll = new QScrollArea;
        scroll->setWidget(content);
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        return scroll;
    };

    // Emitter
    mEmitterTabs->addTab(wrapInScroll(mGeneralInspector), "General");
    mEmitterTabs->addTab(mStripeInspector, "Stripe");
    mEmitterTabs->addTab(wrapInScroll(mLifespanInspector), "Life");
    mEmitterTabs->addTab(wrapInScroll(mTerminationInspector), "Termination");
    mEmitterTabs->addTab(wrapInScroll(mGravityInspector), "Gravity");
    mEmitterTabs->addTab(wrapInScroll(mEmissionInspector), "Emission");
    mEmitterTabs->addTab(wrapInScroll(mVolumeInspector), "Volume");
    mEmitterTabs->addTab(wrapInScroll(mVelocityInspector), "Velocity");
    mEmitterTabs->addTab(wrapInScroll(mTextureInspector), "Texture");
    mEmitterTabs->addTab(wrapInScroll(mColorInspector), "Color");
    mEmitterTabs->addTab(wrapInScroll(mCombinerInspector), "Combiner");
    mEmitterTabs->addTab(wrapInScroll(mAlphaAnimInspector), "Alpha");
    mEmitterTabs->addTab(wrapInScroll(mTransformInspector), "Transform");
    mEmitterTabs->addTab(wrapInScroll(mRotationInspector), "Rotation");
    mEmitterTabs->addTab(wrapInScroll(mScaleAnimInspector), "Scale");

    // Fluctuation
    mFluxTabs->addTab(mFluctuationInspector, "Fluctuation");

    // Field
    mFieldTabs->addTab(mFieldCollisionInspector, "Collision");
    mFieldTabs->addTab(mFieldConvergenceInspector, "Convergence");
    mFieldTabs->addTab(mFieldMagnetInspector, "Magnet");
    mFieldTabs->addTab(mFieldPosAddInspector, "Pos Add");
    mFieldTabs->addTab(mFieldRandomInspector, "Random");
    mFieldTabs->addTab(mFieldSpinInspector, "Spin");

    // Child
    mChildTabs->addTab(mChildGeneralInspector, "General");
    mChildTabs->addTab(mChildColorInspector, "Color");
    mChildTabs->addTab(mChildAlphaInspector, "Alpha");
    mChildTabs->addTab(mChildCombinerInspector, "Combiner");
    mChildTabs->addTab(mChildEmissionInspector, "Emission");
    mChildTabs->addTab(mChildRotationInspector, "Rotation");
    mChildTabs->addTab(mChildScaleInspector, "Scale");
    mChildTabs->addTab(mChildTextureInspector, "Texture");
    mChildTabs->addTab(mChildVelocityInspector, "Velocity");
}

void InspectorPanel::updateTabVisibility() {
    if (!mSelection) {
        return;
    }

    const auto type = mSelection->type();

    switch (type) {
    case Ptcl::Selection::Type::Emitter:
        mTabStack->setCurrentWidget(mEmitterTabs);
        break;
    case Ptcl::Selection::Type::EmitterChild:
        mTabStack->setCurrentWidget(mChildTabs);
        break;
    case Ptcl::Selection::Type::EmitterFlux:
        mTabStack->setCurrentWidget(mFluxTabs);
        break;
    case Ptcl::Selection::Type::EmitterField:
        mTabStack->setCurrentWidget(mFieldTabs);
        break;
    default:
        break;
    }
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
    mStripeInspector->setDocument(document);

    mFluctuationInspector->setDocument(document);

    mFieldCollisionInspector->setDocument(document);
    mFieldConvergenceInspector->setDocument(document);
    mFieldMagnetInspector->setDocument(document);
    mFieldPosAddInspector->setDocument(document);
    mFieldRandomInspector->setDocument(document);
    mFieldSpinInspector->setDocument(document);

    mChildAlphaInspector->setDocument(document);
    mChildColorInspector->setDocument(document);
    mChildCombinerInspector->setDocument(document);
    mChildEmissionInspector->setDocument(document);
    mChildGeneralInspector->setDocument(document);
    mChildRotationInspector->setDocument(document);
    mChildScaleInspector->setDocument(document);
    mChildTextureInspector->setDocument(document);
    mChildVelocityInspector->setDocument(document);

    if (mDocument) {
        QSignalBlocker b1(mProjNameLineEdit);
        mProjNameLineEdit->setText(document->projectName());

        connect(mDocument, &Ptcl::Document::emitterChanged, this, [this](s32 setIndex, s32 emitterIndex) {
            if (!mEmitter || setIndex != mSelection->emitterSetIndex() || emitterIndex != mSelection->emitterIndex()) {
                return;
            }

            populateProperties();
        });

        connect(mDocument, &Ptcl::Document::projectChanged, this, [this]() {
            QSignalBlocker b1(mProjNameLineEdit);
            mProjNameLineEdit->setText(mDocument->projectName());
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

    mFieldCollisionInspector->setSelection(selection);
    mFieldConvergenceInspector->setSelection(selection);
    mFieldMagnetInspector->setSelection(selection);
    mFieldPosAddInspector->setSelection(selection);
    mFieldRandomInspector->setSelection(selection);
    mFieldSpinInspector->setSelection(selection);

    mChildAlphaInspector->setSelection(selection);
    mChildColorInspector->setSelection(selection);
    mChildCombinerInspector->setSelection(selection);
    mChildEmissionInspector->setSelection(selection);
    mChildGeneralInspector->setSelection(selection);
    mChildRotationInspector->setSelection(selection);
    mChildScaleInspector->setSelection(selection);
    mChildTextureInspector->setSelection(selection);
    mChildVelocityInspector->setSelection(selection);

    if (mSelection) {
        connect(selection, &Ptcl::Selection::selectionChanged, this, [this](s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type type) {
            if (!mDocument) {
                mEmitter = nullptr;
                setEnabled(false);
                return;
            }

            mEmitter = mDocument->emitter(setIndex, emitterIndex);
            setEnabled(true);
            populateProperties();
        });
    }
}

void InspectorPanel::populateProperties() {
    if (mLastSelectionType != mSelection->type()) {
        mLastSelectionType = mSelection->type();
        updateTabVisibility();
    }

    // TODO - Stripe should be combined some other tab, this shouldn't be handled by the inspector
    const s32 stripeIdx = mEmitterTabs->indexOf(mStripeInspector);
    const bool hasStripe = mEmitter->hasStripeData();
    mEmitterTabs->setTabVisible(stripeIdx, hasStripe);
}


// ========================================================================== //


} // namespace PtclEditor
