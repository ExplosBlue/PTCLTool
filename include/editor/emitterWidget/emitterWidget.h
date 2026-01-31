#pragma once

#include "editor/childEditor/childEditorWidget.h"
#include "editor/fluctuationEditorWidget.h"
#include "editor/fieldEditor/fieldEditorWidget.h"

#include "ptcl/ptclEmitter.h"
#include "ptcl/ptcl.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QStackedWidget>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidget final : public QWidget {
    Q_OBJECT
public:
    explicit EmitterWidget(QWidget* parent = nullptr);

    void setEmitter(Ptcl::Emitter* emitter);
    void setTextureList(const Ptcl::TextureList* textureList);
    void showStandardEditor();
    void showChildEditor();
    void showFluctuationEditor();
    void showFieldEditor();

    void clear();

signals:
    void textureUpdated(int oldIndex, int index);
    void nameUpdated(const QString& name);
    void emitterTypeChanged();
    void emitterNameChanged();

private:
    void setupStandardLayout(QVBoxLayout* mainLayout);
    void setupConnections();

private:
    class BasicPropertiesWidget;
    class GravityPropertiesWidget;
    class TransformPropertiesWidget;
    class LifespanPropertiesWidget;
    class TerminationPropertiesWidget;
    class EmissionPropertiesWidget;
    class VelocityPropertiesWidget;
    class VolumePropertiesWidget;
    class ColorPropertiesWidget;
    class AlphaPropertiesWidget;
    class RotationPropertiesWidget;
    class ScalePropertiesWidget;
    class TexturePropertiesWidget;
    class CombinerPropertiesWidget;

private:
    Ptcl::Emitter* mEmitterPtr{nullptr};
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

    QStackedWidget* mStackedWidget{nullptr};
    ChildEditorWidget* mChildEditorWidget{nullptr};
    FluctuationEditorWidget* mFluctuationEditorWidget{nullptr};
    FieldEditorWidget* mFieldEditorWidget{nullptr};
};


// ========================================================================== //


} // namespace PtclEditor
