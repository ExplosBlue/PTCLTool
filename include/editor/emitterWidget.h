#ifndef EMITTERWIDGET_H
#define EMITTERWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include "ptcl/ptclEmitter.h"

#include "enumComboBox.h"
#include "sizedSpinBox.h"
#include "vectorSpinBox.h"

#include "rgbaColorWidget.h"
#include "texturePropertiesWidget.h"

#include "colorGradientEditor.h"

namespace PtclEditor {

class EmitterWidget : public QWidget {
    Q_OBJECT
public:
    explicit EmitterWidget(QWidget* parent = nullptr);

    void setEmitter(Ptcl::Emitter* emitter);

    TexturePropertiesWidget& getTextureProperties();

signals:
    void textureUpdated(int oldIndex, int index);

private slots:
    void handleColorChanged();

    void updateColorSection(ColorGradientEditor::HandleType handleType);

private:
    Ptcl::Emitter* mEmitterPtr;

    QGridLayout mMainLayout;

    EnumComboBox<Ptcl::EmitterType> mTypeComboBox;
    SizedSpinBox<u32> mFlagSpinBox;

    SizedSpinBox<u32> mRandomSeedSpinBox;
    QLineEdit mNameLineEdit;

    TexturePropertiesWidget mTextureProperties;

    SizedSpinBox<u8> m_2CSpinBox;
    SizedSpinBox<u8> m_2DSpinBox;
    SizedSpinBox<u8> m_2ESpinBox;
    SizedSpinBox<u8> m_2FSpinBox;
    SizedSpinBox<u8> m_30SpinBox;
    SizedSpinBox<u8> m_31SpinBox;
    SizedSpinBox<u16> m_32SpinBox; // probably padding...

    EnumComboBox<Ptcl::VolumeType> mVolumeTypeComboBox;
    VectorSpinBox<QVector3D> mVolumeRadiusSpinBox;

    QDoubleSpinBox m_44SpinBox;
    QDoubleSpinBox m_48SpinBox;
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

    std::array<RGBAColorWidget, 3> mColorWidgets;

    // _D8

    // ColorSection1, 2, 3
    ColorGradientEditor mColorSections;

    SizedSpinBox<s32> mColorNumRepeatSpinBox;
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

    SizedSpinBox<u32> m_12CSpinBox;
    SizedSpinBox<u32> m_130SpinBox;
    SizedSpinBox<u32> m_134SpinBox;

    VectorSpinBox<QVector3D> mInitRotSpinBox;
    VectorSpinBox<QVector3D> mInitRotRandSpinBox;
    VectorSpinBox<QVector3D> mRotVelSpinBox;
    VectorSpinBox<QVector3D> mRotVelRandSpinBox;

    // _168
    // transformSRT
    // transformRT

    bool mIsPopulating;
};

} // namespace PtclEditor

#endif // EMITTERWIDGET_H
