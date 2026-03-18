#pragma once

#include "editor/childEditor/childEditorWidget.h"

#include "ptcl/ptclDocument.h"
#include "ptcl/ptclEmitter.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QTabWidget>
#include <QWidget>


namespace PtclEditor {

class GeneralEmitterInspector;
class GravityInspector;
class TransformInspector;
class LifespanInspector;
class ScaleAnimInspector;
class TerminationInspector;
class EmissionInspector;
class VelocityInspector;
class VolumeInspector;
class RotationInspector;
class AlphaAnimInspector;
class CombinerInspector;
class ColorInspector;
class TextureInspector;
class FluctuationInspector;
class StripeInspector;

class FieldCollisionInspector;
class FieldConvergenceInspector;
class FieldMagnetInspector;
class FieldPosAddInspector;
class FieldRandomInspector;
class FieldSpinInspector;


// ========================================================================== //


class InspectorPanel final : public QWidget {
    Q_OBJECT
public:
    explicit InspectorPanel(QWidget* parent = nullptr);

    void setDocument(Ptcl::Document* document);
    void setSelection(Ptcl::Selection* selection);

signals:
    void complexFlagsChanged();
    void propertiesChanged();

private:
    void setupConnections();
    void populateProperties();

    void buildTabs();
    void updateTabVisibility();

private:
    Ptcl::Document* mDocument{nullptr};
    const Ptcl::Selection* mSelection{nullptr};
    Ptcl::Emitter* mEmitter{nullptr};

    GeneralEmitterInspector* mGeneralInspector{nullptr};
    GravityInspector* mGravityInspector{nullptr};
    TransformInspector* mTransformInspector{nullptr};
    LifespanInspector* mLifespanInspector{nullptr};
    TerminationInspector* mTerminationInspector{nullptr};
    EmissionInspector* mEmissionInspector{nullptr};
    VelocityInspector* mVelocityInspector{nullptr};
    VolumeInspector* mVolumeInspector{nullptr};
    ColorInspector* mColorInspector{nullptr};
    AlphaAnimInspector* mAlphaAnimInspector{nullptr};
    RotationInspector* mRotationInspector{nullptr};
    ScaleAnimInspector* mScaleAnimInspector{nullptr};
    TextureInspector* mTextureInspector{nullptr};
    CombinerInspector* mCombinerInspector{nullptr};
    StripeInspector* mStripeInspector{nullptr};
    FluctuationInspector* mFluctuationInspector{nullptr};

    ChildEditorWidget* mChildEditorWidget{nullptr};

    FieldCollisionInspector* mFieldCollisionInspector{nullptr};
    FieldConvergenceInspector* mFieldConvergenceInspector{nullptr};
    FieldMagnetInspector* mFieldMagnetInspector{nullptr};
    FieldPosAddInspector* mFieldPosAddInspector{nullptr};
    FieldRandomInspector* mFieldRandomInspector{nullptr};
    FieldSpinInspector* mFieldSpinInspector{nullptr};

    QStackedWidget* mTabStack{nullptr};
    QTabWidget* mEmitterTabs{nullptr};
    QTabWidget* mChildTabs{nullptr};
    QTabWidget* mFieldTabs{nullptr};
    QTabWidget* mFluxTabs{nullptr};

    Ptcl::Selection::Type mLastSelectionType{};
};


// ========================================================================== //


} // namespace PtclEditor
