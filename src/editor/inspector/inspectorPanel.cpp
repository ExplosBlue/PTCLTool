
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

#include "editor/inspector/field/fieldCollisionInspector.h"
#include "editor/inspector/field/fieldConvergenceInspector.h"
#include "editor/inspector/field/fieldMagnetInspector.h"
#include "editor/inspector/field/fieldPosAddInspector.h"
#include "editor/inspector/field/fieldRandomInspector.h"
#include "editor/inspector/field/fieldSpinInspector.h"

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

    mFieldCollisionInspector = new FieldCollisionInspector(this);
    mFieldConvergenceInspector = new FieldConvergenceInspector(this);
    mFieldMagnetInspector = new FieldMagnetInspector(this);
    mFieldPosAddInspector = new FieldPosAddInspector(this);
    mFieldRandomInspector = new FieldRandomInspector(this);
    mFieldSpinInspector = new FieldSpinInspector(this);

    mChildEditorWidget = new ChildEditorWidget(this);
    mFluctuationInspector = new FluctuationInspector(this);

    mTabWidget = new QTabWidget(this);
    mTabWidget->setTabPosition(QTabWidget::West);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(mTabWidget);
    layout->setContentsMargins(0, 0, 0, 0);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    buildTabs();
    setupConnections();
}

s32 InspectorPanel::addTab(QWidget* widget, const QString& label, TabId id) {
    s32 idx = mTabWidget->addTab(widget, label);
    mTabWidget->tabBar()->setTabData(idx, QVariant::fromValue(id));
    mTabIndex[id] = idx;
    return idx;
}

void InspectorPanel::buildTabs() {
    // Emitter
    addTab(mGeneralInspector, "General", TabId::General);
    addTab(mStripeInspector, "Stripe", TabId::Stripe);
    addTab(mLifespanInspector, "Life", TabId::Lifespan);
    addTab(mTerminationInspector, "Termination", TabId::Termination);
    addTab(mGravityInspector, "Gravity", TabId::Gravity);
    addTab(mEmissionInspector, "Emission", TabId::Emission);
    addTab(mVolumeInspector, "Volume", TabId::Volume);
    addTab(mVelocityInspector, "Velocity", TabId::Velocity);
    addTab(mTextureInspector, "Texture", TabId::Texture);
    addTab(mColorInspector, "Color", TabId::Color);
    addTab(mCombinerInspector, "Combiner", TabId::Combiner);
    addTab(mAlphaAnimInspector, "Alpha", TabId::AlphaAnim);
    addTab(mTransformInspector, "Transform", TabId::Transform);
    addTab(mRotationInspector, "Rotation", TabId::Rotation);
    addTab(mScaleAnimInspector, "Scale", TabId::ScaleAnim);

    // Fluctuation
    addTab(mFluctuationInspector, "Fluctuation",TabId::Fluctuation);

    // Field
    addTab(mFieldCollisionInspector, "Collision", TabId::FieldCollision);
    addTab(mFieldConvergenceInspector, "Convergence", TabId::FieldConvergence);
    addTab(mFieldMagnetInspector, "Magnet", TabId::FieldMagnet);
    addTab(mFieldPosAddInspector, "Pos Add", TabId::FieldPosAdd);
    addTab(mFieldRandomInspector, "Random", TabId::FieldRandom);
    addTab(mFieldSpinInspector, "Spin", TabId::FieldSpin);

    // Child
    addTab(mChildEditorWidget, "Child", TabId::Child);
    // TODO
}

void InspectorPanel::updateTabVisibility() {
    if (!mSelection) {
        return;
    }

    TabId currentId{};
    bool hasCurrent = false;

    s32 currentIndex = mTabWidget->currentIndex();
    if (currentIndex >= 0) {
        QVariant data = mTabWidget->tabBar()->tabData(currentIndex);
        if (data.isValid()) {
            currentId = data.value<TabId>();
            hasCurrent = true;
        }
    }

    const auto type = mSelection->type();

    auto setVisible = [this](TabId id, bool visible) {
        if (!mTabIndex.contains(id)) {
            return;
        }
        mTabWidget->setTabVisible(mTabIndex[id], visible);
    };

    for (auto [id, _] : mTabIndex.asKeyValueRange()) {
        setVisible(id, false);
    }

    switch (type) {
    case Ptcl::Selection::Type::Emitter:
        setVisible(TabId::General, true);
        setVisible(TabId::Gravity, true);
        setVisible(TabId::Transform, true);
        setVisible(TabId::Lifespan, true);
        setVisible(TabId::Termination, true);
        setVisible(TabId::Emission, true);
        setVisible(TabId::Velocity, true);
        setVisible(TabId::Volume, true);
        setVisible(TabId::Color, true);
        setVisible(TabId::AlphaAnim, true);
        setVisible(TabId::Rotation, true);
        setVisible(TabId::ScaleAnim, true);
        setVisible(TabId::Texture, true);
        setVisible(TabId::Combiner, true);

        if (mEmitter && mEmitter->hasStripeData()) {
            setVisible(TabId::Stripe, true);
        }
        break;
    case Ptcl::Selection::Type::EmitterChild:
        setVisible(TabId::Child, true);
        break;
    case Ptcl::Selection::Type::EmitterFlux:
        setVisible(TabId::Fluctuation, true);
        break;
    case Ptcl::Selection::Type::EmitterField:
        setVisible(TabId::FieldCollision, true);
        setVisible(TabId::FieldConvergence, true);
        setVisible(TabId::FieldMagnet, true);
        setVisible(TabId::FieldPosAdd, true);
        setVisible(TabId::FieldRandom, true);
        setVisible(TabId::FieldSpin, true);
        break;
    default:
        break;
    }

    if (hasCurrent && mTabIndex.contains(currentId)) {
        s32 index = mTabIndex[currentId];
        if (mTabWidget->isTabVisible(index)) {
            mTabWidget->setCurrentIndex(index);
            return;
        }
    }

    for (s32 i = 0; i < mTabWidget->count(); ++i) {
        if (mTabWidget->isTabVisible(i)) {
            mTabWidget->setCurrentIndex(i);
            break;
        }
    }
}

void InspectorPanel::setupConnections() {
    // Child Editor Widget
    connect(mChildEditorWidget, &ChildEditorWidget::flagsUpdated, this, [this](const BitFlag<Ptcl::ChildFlag>& childFlags) {
        if (!mEmitter) { return; }
        mEmitter->setChildFlags(childFlags);
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

    mFieldCollisionInspector->setDocument(document);
    mFieldConvergenceInspector->setDocument(document);
    mFieldMagnetInspector->setDocument(document);
    mFieldPosAddInspector->setDocument(document);
    mFieldRandomInspector->setDocument(document);
    mFieldSpinInspector->setDocument(document);

    if (mDocument) {
        connect(mDocument, &Ptcl::Document::emitterChanged, this, [this](s32 setIndex, s32 emitterIndex) {
            if (!mEmitter) {
                return;
            }

            if (setIndex != mSelection->emitterSetIndex() || emitterIndex != mSelection->emitterIndex()) {
                return;
            }

            populateProperties();

            if (mLastEmitterType != mEmitter->type() || mLastEmitterHasStripe != mEmitter->hasStripeData()) {
                updateTabVisibility();

                mLastEmitterType = mEmitter->type();
                mLastEmitterHasStripe = mEmitter->hasStripeData();
            }
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

    if (mSelection) {
        connect(selection, &Ptcl::Selection::selectionChanged, this, [this](s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type type) {
            if (!mDocument) {
                mEmitter = nullptr;
                setEnabled(false);
                return;
            }

            mEmitter = mDocument->emitter(setIndex, emitterIndex);

            updateTabVisibility();

            // TODO: Have child widgets handle this themselves
            mChildEditorWidget->setTextureList(mTextureList);

            setEnabled(true);
            populateProperties();
        });
    }
}

void InspectorPanel::populateProperties() {
    QSignalBlocker b15(mChildEditorWidget);
    QSignalBlocker b18(mStripeInspector);

    mChildEditorWidget->setChildData(&mEmitter->childData(), mEmitter->complexProperties().childFlags);
    mChildEditorWidget->setParentColor0(mEmitter->primaryColor());
}


// ========================================================================== //


} // namespace PtclEditor
