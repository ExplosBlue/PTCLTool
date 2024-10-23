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

#include "sizedSpinBox.h"

namespace PtclEditor {

class EmitterWidget : public QWidget
{
    Q_OBJECT
public:
    explicit EmitterWidget(QWidget* parent = nullptr);

    void setEmitter(Ptcl::Emitter* emitter);

signals:

private:
    Ptcl::Emitter* mEmitterPtr;

    QGridLayout mMainLayout;

    QComboBox mTypeComboBox;
    // QSpinBox mFlagSpinBox;
    SizedSpinBox<u32> mFlagSpinBox;

    SizedSpinBox<u32> mRandomSeedSpinBox;
    QLineEdit mNameLineEdit;

    // TODO: Texture Info (move to dedicated widget?)
    QLabel mImageLabel;
    QLabel mImageInfoLabel;

    // TODO: More

    SizedSpinBox<u8> m_2CSpinBox;
    SizedSpinBox<u8> m_2DSpinBox;
    SizedSpinBox<u8> m_2ESpinBox;
    SizedSpinBox<u8> m_2FSpinBox;
    SizedSpinBox<u8> m_30SpinBox;
    SizedSpinBox<u8> m_31SpinBox;
    SizedSpinBox<u16> m_32SpinBox; // probably padding...

    QLabel mVolumeTypeLabel;
    QLabel mVolumeRadiusLabel;

    QDoubleSpinBox m_44SpinBox;
    QDoubleSpinBox m_48SpinBox;
    QDoubleSpinBox m_4CSpinBox;

    // std::array<u32, 13> m_50;

    SizedSpinBox<s32> mPtclLifeSpinBox;
    SizedSpinBox<u32> m_88SpinBox;
    QDoubleSpinBox m_8CSpinBox;

    // std::array<u32, 3> m_90;
    // f32 m_9C;
    // f32 m_A0;
    // f32 m_A4;
    // std::array<QColor, 3> mColors;
    // std::array<u32, 3> m_D8;
    // s32 mColorSection1; // Should these be QColor?
    // s32 mColorSection2; // Should these be QColor?
    // s32 mColorSection3; // Should these be QColor?
    // s32 mColorNumRepeat;
    // f32 mInitAlpha;
    // std::array<u32, 4> m_F8;
    // QVector2D mInitScale;
    // std::array<u32, 24> m_110;
    // QMatrix3x4 m_170; // SRT related
    // QMatrix3x4 m_1A0; // RT related
    // std::array<u32, 9> m_1D0;

};

} // namespace PtclEditor

#endif // EMITTERWIDGET_H
