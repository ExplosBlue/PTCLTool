#pragma once

#include "ptcl/ptclEmitter.h"
#include "editor/alphaPropertiesWidget.h"
#include "editor/colorPropertiesWidget.h"
#include "editor/emissionPropertiesWidget.h"
#include "editor/enumComboBox.h"
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

    TexturePropertiesWidget& getTextureProperties();

signals:
    void textureUpdated(int oldIndex, int index);
    void nameUpdated(const QString& name);

private:
    Ptcl::Emitter* mEmitterPtr;

    QGridLayout mMainLayout;

    EnumComboBox<Ptcl::EmitterType> mTypeComboBox;

    QComboBox mRandomSeedMode;
    SizedSpinBox<u32> mRandomSeedSpinBox;
    QLineEdit mNameLineEdit;

    TexturePropertiesWidget mTextureProperties;

    SizedSpinBox<u8> m_2CSpinBox;
    SizedSpinBox<u8> m_2DSpinBox;
    SizedSpinBox<u8> m_2ESpinBox;
    SizedSpinBox<u8> m_2FSpinBox;
    SizedSpinBox<u8> m_30SpinBox;
    SizedSpinBox<u8> m_31SpinBox;

    // _70

    QDoubleSpinBox m_8CSpinBox;
    SizedSpinBox<u32> m_90SpinBox;
    EnumComboBox<Ptcl::BillboardType> mBillboardComboBox;
    SizedSpinBox<u32> m_98SpinBox;
    QDoubleSpinBox m_9CSpinBox;
    QDoubleSpinBox m_A0SpinBox;
    QDoubleSpinBox m_A4SpinBox;

    // _D8

    EnumComboBox<Ptcl::CombinerType> mCombinerTypeSpinBox;
    EnumComboBox<Ptcl::FollowType> mFollowTypeSpinBox;
    SizedSpinBox<u32> m_134SpinBox;

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


    // _168

    bool mIsPopulating;
};


// ========================================================================== //


} // namespace PtclEditor
