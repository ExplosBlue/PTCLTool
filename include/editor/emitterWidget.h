#pragma once

#include "ptcl/ptclEmitter.h"
#include "editor/colorPropertiesWidget.h"
#include "editor/enumComboBox.h"
#include "editor/rotationPropertiesWidget.h"
#include "editor/sizedSpinBox.h"
#include "editor/texturePropertiesWidget.h"
#include "editor/vectorSpinBox.h"
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
    SizedSpinBox<u8> m_33SpinBox;

    VolumePropertiesWidget mVolumeProperties;

    QDoubleSpinBox mFigureVelSpinBox;
    VectorSpinBox<QVector3D> mEmitterVelDirSpinBox;
    QDoubleSpinBox m_5CSpinBox;
    QDoubleSpinBox mInitVelRndSpinBox;
    VectorSpinBox<QVector3D> mSpreadVecSpinBox;

    // _70

    SizedSpinBox<u32> m_80SpinBox;
    SizedSpinBox<s32> mPtclLifeSpinBox;
    SizedSpinBox<u32> mPtclLifeRandSpinBox;
    QDoubleSpinBox m_8CSpinBox;
    SizedSpinBox<u32> m_90SpinBox;
    EnumComboBox<Ptcl::BillboardType> mBillboardComboBox;
    SizedSpinBox<u32> m_98SpinBox;
    QDoubleSpinBox m_9CSpinBox;
    QDoubleSpinBox m_A0SpinBox;
    QDoubleSpinBox m_A4SpinBox;

    // _D8

    ColorPropertiesWidget mColorProperties;

    QDoubleSpinBox mInitAlphaSpinBox;
    QDoubleSpinBox mDiffAlpha21SpinBox;
    QDoubleSpinBox mDiffAlpha32SpinBox;
    SizedSpinBox<s32> mAlphaSection1SpinBox;
    SizedSpinBox<s32> mAlphaSection2SpinBox;

    VectorSpinBox<QVector2D> mInitScaleSpinBox;
    VectorSpinBox<QVector2D> mDiffScale21SpinBox;
    VectorSpinBox<QVector2D> mDiffScale32SpinBox;
    SizedSpinBox<s32> mScaleSection1SpinBox;
    SizedSpinBox<s32> mScaleSection2SpinBox;
    QDoubleSpinBox mScaleRandSpinBox;

    RotationPropertiesWidget mRotationProperties;

    EnumComboBox<Ptcl::CombinerType> mCombinerTypeSpinBox;
    EnumComboBox<Ptcl::FollowType> mFollowTypeSpinBox;
    SizedSpinBox<u32> m_134SpinBox;



    // _168
    // transformSRT
    // transformRT

    bool mIsPopulating;
};


// ========================================================================== //


} // namespace PtclEditor
