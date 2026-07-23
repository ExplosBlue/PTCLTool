#pragma once

#include "ptcl/ptclDocument.h"
#include "ptcl/ptclEmitter.h"

#include <QLineEdit>
#include <QStackedWidget>
#include <QTabWidget>
#include <QWidget>


namespace PtclEditor {

class EmitterSetInspector;

class GeneralEmitterInspector;
class PhysicsInspector;
class ShapeInspector;
class LifespanInspector;
class RotationScaleInspector;
class CombinerInspector;
class ColorInspector;
class TextureInspector;

class FluctuationInspector;

class FieldCollisionInspector;
class FieldConvergenceInspector;
class FieldMagnetInspector;
class FieldPosAddInspector;
class FieldRandomInspector;
class FieldSpinInspector;

class ChildColorInspector;
class ChildCombinerInspector;
class ChildEmissionInspector;
class ChildGeneralInspector;
class ChildRotationScaleInspector;
class ChildTextureInspector;
class ChildVelocityInspector;


// ========================================================================== //


class InspectorPanel final : public QWidget {
    Q_OBJECT
public:
    explicit InspectorPanel(QWidget* parent = nullptr);

    void setDocument(Ptcl::Document* document);
    void setSelection(Ptcl::Selection* selection);

private:
    void setupConnections();
    void populateProperties();

    void buildTabs();
    void updateTabVisibility();

private:
    Ptcl::Document* mDocument{nullptr};
    const Ptcl::Selection* mSelection{nullptr};
    const Ptcl::Emitter* mEmitter{nullptr};

    QLineEdit* mProjNameLineEdit{nullptr};

    EmitterSetInspector* mEmitterSetInspector{nullptr};

    GeneralEmitterInspector* mGeneralInspector{nullptr};
    PhysicsInspector* mPhysicsInspector{nullptr};
    ShapeInspector* mShapeInspector{nullptr};
    LifespanInspector* mLifespanInspector{nullptr};
    ColorInspector* mColorInspector{nullptr};
    RotationScaleInspector* mRotationScaleInspector{nullptr};
    TextureInspector* mTextureInspector{nullptr};
    CombinerInspector* mCombinerInspector{nullptr};

    FluctuationInspector* mFluctuationInspector{nullptr};

    FieldCollisionInspector* mFieldCollisionInspector{nullptr};
    FieldConvergenceInspector* mFieldConvergenceInspector{nullptr};
    FieldMagnetInspector* mFieldMagnetInspector{nullptr};
    FieldPosAddInspector* mFieldPosAddInspector{nullptr};
    FieldRandomInspector* mFieldRandomInspector{nullptr};
    FieldSpinInspector* mFieldSpinInspector{nullptr};

    ChildColorInspector* mChildColorInspector{nullptr};
    ChildCombinerInspector* mChildCombinerInspector{nullptr};
    ChildEmissionInspector* mChildEmissionInspector{nullptr};
    ChildGeneralInspector* mChildGeneralInspector{nullptr};
    ChildRotationScaleInspector* mChildRotationScaleInspector{nullptr};
    ChildTextureInspector* mChildTextureInspector{nullptr};
    ChildVelocityInspector* mChildVelocityInspector{nullptr};

    QStackedWidget* mTabStack{nullptr};
    QTabWidget* mEmitterSetTabs{nullptr};
    QTabWidget* mEmitterTabs{nullptr};
    QTabWidget* mChildTabs{nullptr};
    QTabWidget* mFieldTabs{nullptr};
    QTabWidget* mFluxTabs{nullptr};

    Ptcl::Selection::Type mLastSelectionType{};
};


// ========================================================================== //


} // namespace PtclEditor
