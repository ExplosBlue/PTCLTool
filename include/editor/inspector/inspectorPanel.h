#pragma once

#include "editor/childEditor/childEditorWidget.h"
#include "editor/components/collapsibleWidget.h"
#include "editor/fieldEditor/fieldEditorWidget.h"

#include "ptcl/ptclDocument.h"
#include "ptcl/ptclEmitter.h"
#include "ptcl/ptcl.h"

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


// ========================================================================== //


class InspectorPanel final : public QWidget {
    Q_OBJECT
public:
    explicit InspectorPanel(QWidget* parent = nullptr);

    void setDocument(Ptcl::Document* document);
    void setSelection(Ptcl::Selection* selection);

signals:
    void nameUpdated(const QString& name);
    void emitterTypeChanged();
    void emitterNameChanged();
    void complexFlagsChanged();
    void propertiesChanged();

private:
    void setupConnections();
    void populateProperties();

    void rebuildTabs();
    void buildEmitterTabs();
    void buildChildTabs();
    void buildFluxTabs();
    void buildFieldTabs();

    QWidget* wrapInScroll(QWidget* widget);

private:
    Ptcl::Document* mDocument{nullptr};
    const Ptcl::Selection* mSelection{nullptr};
    Ptcl::Emitter* mEmitter{nullptr};

    const Ptcl::TextureList* mTextureList{nullptr};

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

    QTabWidget* mTabWidget{nullptr};

    ChildEditorWidget* mChildEditorWidget{nullptr};
    FieldEditorWidget* mFieldEditorWidget{nullptr};

    CollapsibleWidget* mStripeSection{nullptr};
};


// ========================================================================== //


} // namespace PtclEditor
