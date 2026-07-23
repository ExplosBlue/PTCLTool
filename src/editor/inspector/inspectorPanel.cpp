
#include "editor/inspector/inspectorPanel.h"

#include "editor/inspector/emitterSetInspector.h"

#include "editor/inspector/generalEmitterInspector.h"
#include "editor/inspector/colorInspector.h"
#include "editor/inspector/combinerInspector.h"
#include "editor/inspector/lifespanInspector.h"
#include "editor/inspector/physicsInspector.h"
#include "editor/inspector/rotationScaleInspector.h"
#include "editor/inspector/textureInspector.h"
#include "editor/inspector/shapeInspector.h"

#include "editor/inspector/fluctuationInspector.h"

#include "editor/inspector/child/childColorInspector.h"
#include "editor/inspector/child/childCombinerInspector.h"
#include "editor/inspector/child/childEmissionInspector.h"
#include "editor/inspector/child/childGeneralInspector.h"
#include "editor/inspector/child/childRotationScaleInspector.h"
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

    mEmitterSetInspector = new EmitterSetInspector(this);

    mGeneralInspector = new GeneralEmitterInspector(this);
    mPhysicsInspector = new PhysicsInspector(this);
    mShapeInspector = new ShapeInspector(this);
    mLifespanInspector = new LifespanInspector(this);
    mRotationScaleInspector = new RotationScaleInspector(this);
    mColorInspector = new ColorInspector(this);
    mTextureInspector = new TextureInspector(this);
    mCombinerInspector = new CombinerInspector(this);

    mFieldCollisionInspector = new FieldCollisionInspector(this);
    mFieldConvergenceInspector = new FieldConvergenceInspector(this);
    mFieldMagnetInspector = new FieldMagnetInspector(this);
    mFieldPosAddInspector = new FieldPosAddInspector(this);
    mFieldRandomInspector = new FieldRandomInspector(this);
    mFieldSpinInspector = new FieldSpinInspector(this);

    mChildColorInspector = new ChildColorInspector(this);
    mChildCombinerInspector = new ChildCombinerInspector(this);
    mChildEmissionInspector = new ChildEmissionInspector(this);
    mChildGeneralInspector = new ChildGeneralInspector(this);
    mChildRotationScaleInspector = new ChildRotationScaleInspector(this);
    mChildTextureInspector = new ChildTextureInspector(this);
    mChildVelocityInspector = new ChildVelocityInspector(this);

    mFluctuationInspector = new FluctuationInspector(this);

    // Project Properties
    auto* projectPropertiesLayout = new QHBoxLayout;
    projectPropertiesLayout->addWidget(new QLabel("Project Name"));
    projectPropertiesLayout->addWidget(mProjNameLineEdit);

    mTabStack = new QStackedWidget(this);

    mEmitterSetTabs = new QTabWidget(this);
    mEmitterSetTabs->setTabPosition(QTabWidget::West);
    mTabStack->addWidget(mEmitterSetTabs);

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
    // layout->setContentsMargins(0, 0, 0, 0);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    buildTabs();
}

void InspectorPanel::buildTabs() {
    auto wrapInScroll = [](QWidget* content) {
        auto* scroll = new QScrollArea;
        scroll->setWidget(content);
        scroll->setWidgetResizable(true);
        scroll->setFrameShape(QFrame::NoFrame);
        return scroll;
    };

    // EmitterSet
    mEmitterSetTabs->addTab(mEmitterSetInspector, "EmitterSet");

    // Emitter
    mEmitterTabs->addTab(wrapInScroll(mGeneralInspector), "General");
    mEmitterTabs->addTab(wrapInScroll(mLifespanInspector), "Life");
    mEmitterTabs->addTab(wrapInScroll(mPhysicsInspector), "Physics");
    mEmitterTabs->addTab(wrapInScroll(mShapeInspector), "Shape");
    mEmitterTabs->addTab(wrapInScroll(mTextureInspector), "Texture");
    mEmitterTabs->addTab(wrapInScroll(mColorInspector), "Color");
    mEmitterTabs->addTab(wrapInScroll(mCombinerInspector), "Combiner");
    mEmitterTabs->addTab(wrapInScroll(mRotationScaleInspector), "Transform");

    // Fluctuation
    mFluxTabs->addTab(wrapInScroll(mFluctuationInspector), "Fluctuation");

    // Field
    mFieldTabs->addTab(wrapInScroll(mFieldCollisionInspector), "Collision");
    mFieldTabs->addTab(wrapInScroll(mFieldConvergenceInspector), "Convergence");
    mFieldTabs->addTab(wrapInScroll(mFieldMagnetInspector), "Magnet");
    mFieldTabs->addTab(wrapInScroll(mFieldPosAddInspector), "Pos Add");
    mFieldTabs->addTab(wrapInScroll(mFieldRandomInspector), "Random");
    mFieldTabs->addTab(wrapInScroll(mFieldSpinInspector), "Spin");

    // Child
    mChildTabs->addTab(wrapInScroll(mChildGeneralInspector), "General");
    mChildTabs->addTab(wrapInScroll(mChildColorInspector), "Color");
    mChildTabs->addTab(wrapInScroll(mChildCombinerInspector), "Combiner");
    mChildTabs->addTab(wrapInScroll(mChildEmissionInspector), "Emission");
    mChildTabs->addTab(wrapInScroll(mChildRotationScaleInspector), "Transform");
    mChildTabs->addTab(wrapInScroll(mChildTextureInspector), "Texture");
    mChildTabs->addTab(wrapInScroll(mChildVelocityInspector), "Velocity");
}

void InspectorPanel::updateTabVisibility() {
    if (!mSelection) {
        return;
    }

    const auto type = mSelection->type();

    switch (type) {
    case Ptcl::Selection::Type::EmitterSet:
        mTabStack->setCurrentWidget(mEmitterSetTabs);
        break;
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
    mLastSelectionType = Ptcl::Selection::Type::None;

    mEmitterSetInspector->setDocument(document);

    mGeneralInspector->setDocument(document);
    mPhysicsInspector->setDocument(document);
    mShapeInspector->setDocument(document);
    mLifespanInspector->setDocument(document);
    mRotationScaleInspector->setDocument(document);
    mCombinerInspector->setDocument(document);
    mColorInspector->setDocument(document);
    mTextureInspector->setDocument(document);

    mFluctuationInspector->setDocument(document);

    mFieldCollisionInspector->setDocument(document);
    mFieldConvergenceInspector->setDocument(document);
    mFieldMagnetInspector->setDocument(document);
    mFieldPosAddInspector->setDocument(document);
    mFieldRandomInspector->setDocument(document);
    mFieldSpinInspector->setDocument(document);

    mChildColorInspector->setDocument(document);
    mChildCombinerInspector->setDocument(document);
    mChildEmissionInspector->setDocument(document);
    mChildGeneralInspector->setDocument(document);
    mChildRotationScaleInspector->setDocument(document);
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

    mEmitterSetInspector->setSelection(selection);

    mGeneralInspector->setSelection(selection);
    mPhysicsInspector->setSelection(selection);
    mShapeInspector->setSelection(selection);
    mLifespanInspector->setSelection(selection);
    mRotationScaleInspector->setSelection(selection);
    mCombinerInspector->setSelection(selection);
    mColorInspector->setSelection(selection);
    mTextureInspector->setSelection(selection);
    mFluctuationInspector->setSelection(selection);

    mFieldCollisionInspector->setSelection(selection);
    mFieldConvergenceInspector->setSelection(selection);
    mFieldMagnetInspector->setSelection(selection);
    mFieldPosAddInspector->setSelection(selection);
    mFieldRandomInspector->setSelection(selection);
    mFieldSpinInspector->setSelection(selection);

    mChildColorInspector->setSelection(selection);
    mChildCombinerInspector->setSelection(selection);
    mChildEmissionInspector->setSelection(selection);
    mChildGeneralInspector->setSelection(selection);
    mChildRotationScaleInspector->setSelection(selection);
    mChildTextureInspector->setSelection(selection);
    mChildVelocityInspector->setSelection(selection);

    if (mSelection) {
        connect(selection, &Ptcl::Selection::selectionChanged, this, [this](s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type /*type*/) {
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
    if (!mSelection || !mEmitter) {
        return;
    }

    if (mLastSelectionType != mSelection->type()) {
        mLastSelectionType = mSelection->type();
        updateTabVisibility();
    }
}


// ========================================================================== //


} // namespace PtclEditor
