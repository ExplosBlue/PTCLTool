#ifndef EMITTERWIDGET_H
#define EMITTERWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>

#include "ptcl/ptcl.h"

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

    QLabel mTypeLabel;
    QLabel mFlagLabel;
    QLabel mRandomSeedLabel;
    QLabel mNameLabel;

    // TODO: Texture Info (move to dedicated widget?)
    QLabel mImageLabel;
    QLabel mImageInfoLabel;

    // TODO: More

    QLabel m_2CLabel;
    QLabel m_2DLabel;
    QLabel m_2ELabel;
    QLabel m_2FLabel;
    QLabel m_30Label;
    QLabel m_31Label;
    QLabel m_32Label; // probably padding...

    QLabel mVolumeTypeLabel;
    QLabel mVolumeRadiusLabel;

    // f32 m_44;
    // f32 m_48;
    // f32 m_4C;
    // std::array<u32, 13> m_50;
    // s32 mPtclLife;
    // u32 m_88;
    // f32 m_8C;
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

#endif // EMITTERWIDGET_H
