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

class BasicPropertiesWidget;
class GravityPropertiesWidget;
class TransformPropertiesWidget;
class LifespanPropertiesWidget;
class ScalePropertiesWidget;
class TerminationPropertiesWidget;
class EmissionPropertiesWidget;
class VelocityPropertiesWidget;
class VolumePropertiesWidget;
class RotationPropertiesWidget;
class AlphaPropertiesWidget;
class CombinerPropertiesWidget;
class ColorPropertiesWidget;
class TexturePropertiesWidget;
class FluctuationEditorWidget;
class StripeEditorWidget;


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

    BasicPropertiesWidget* mBasicProperties{nullptr};
    GravityPropertiesWidget* mGravityProperties{nullptr};
    TransformPropertiesWidget* mTransformProperties{nullptr};
    LifespanPropertiesWidget* mLifespanProperties{nullptr};
    TerminationPropertiesWidget* mTerminationProperties{nullptr};
    EmissionPropertiesWidget* mEmissionProperties{nullptr};
    VelocityPropertiesWidget* mVelocityProperties{nullptr};
    VolumePropertiesWidget* mVolumeProperties{nullptr};
    ColorPropertiesWidget* mColorProperties{nullptr};
    AlphaPropertiesWidget* mAlphaProperties{nullptr};
    RotationPropertiesWidget* mRotationProperties{nullptr};
    ScalePropertiesWidget* mScaleProperties{nullptr};
    TexturePropertiesWidget* mTextureProperties{nullptr};
    CombinerPropertiesWidget* mCombinerProperties{nullptr};
    StripeEditorWidget* mStripeEditorWidget{nullptr};
    FluctuationEditorWidget* mFluctuationEditorWidget{nullptr};

    QTabWidget* mTabWidget{nullptr};

    ChildEditorWidget* mChildEditorWidget{nullptr};
    FieldEditorWidget* mFieldEditorWidget{nullptr};

    CollapsibleWidget* mStripeSection{nullptr};
};


// ========================================================================== //


} // namespace PtclEditor
