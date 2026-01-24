#pragma once

#include "ptcl/ptclEmitter.h"
#include "editor/alphaPropertiesWidget.h"
#include "editor/basicPropertiesWidget.h"
#include "editor/colorPropertiesWidget.h"
#include "editor/combinerPropertiesWidget.h"
#include "editor/emissionPropertiesWidget.h"
#include "editor/gravityPropertiesWidget.h"
#include "editor/lifespanPropertiesWidget.h"
#include "editor/rotationPropertiesWidget.h"
#include "editor/scalePropertiesWidget.h"
#include "editor/sizedSpinBox.h"
#include "editor/terminationPropertiesWidget.h"
#include "editor/texturePropertiesWidget.h"
#include "editor/transformPropertiesWidget.h"
#include "editor/velocityPropertiesWidget.h"
#include "editor/volumePropertiesWidget.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidget : public QWidget {
    Q_OBJECT
public:
    explicit EmitterWidget(QWidget* parent = nullptr);

    void setEmitter(Ptcl::Emitter* emitter);
    void setTextureList(const Ptcl::TextureList* textureList);

signals:
    void textureUpdated(int oldIndex, int index);
    void nameUpdated(const QString& name);

private:
    Ptcl::Emitter* mEmitterPtr;
    const Ptcl::TextureList* mTextureList{nullptr};

    QGridLayout mMainLayout;

    SizedSpinBox<u8> m_2DSpinBox;

    BasicPropertiesWidget mBasicProperties;
    GravityPropertiesWidget mGravityProperties;
    TransformPropertiesWidget mTransformProperties;
    LifespanPropertiesWidget mLifespanProperties;
    TerminationPropertiesWidget mTerminationProperties;
    EmissionPropertiesWidget mEmissionProperties;
    VelocityPropertiesWidget mVelocityProperties;
    VolumePropertiesWidget mVolumeProperties;
    ColorPropertiesWidget mColorProperties;
    AlphaPropertiesWidget mAlphaProperties;
    RotationPropertiesWidget mRotationProperties;
    ScalePropertiesWidget mScaleProperties;
    TexturePropertiesWidget mTextureProperties;
    CombinerPropertiesWidget mCombinerProperties;

    bool mIsPopulating;
};


// ========================================================================== //


} // namespace PtclEditor
