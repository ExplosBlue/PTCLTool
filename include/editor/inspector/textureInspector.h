#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/frameTimeline.h"
#include "editor/components/sizedSpinBox.h"
#include "editor/components/startframelist.h"
#include "editor/components/texturePreview.h"
#include "editor/inspector/inspectorWidgetBase.h"
#include "ptcl/ptclEnum.h"

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class TextureInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    inline s32 animModeToFreq(FrameAnimMode mode) {
        return static_cast<s32>(mode);
    }

    inline FrameAnimMode freqToAnimMode(s32 value) {
        return (value > 0) ? FrameAnimMode::FixedSpeed : FrameAnimMode::FitToLife;
    }

public:
    explicit TextureInspector(QWidget* parent = nullptr);

private slots:
    void changeTexture();

private:
    void populateProperties() final;
    void setupConnections();

    std::optional<Math::Vector2f> calcFrameUVOffset(s32 frame) const;
    QImage getFrameTexture(s32 frame) const;
    QImage applyUVRepetition(const QImage& image, f32 repeatX, f32 repeatY) const;

    s32 maxFrameCount() const;

    void updateFramesWarning();
    void updateDescription();

private:
    EnumComboBox<Ptcl::TextureWrap> mWrapTComboBox{};
    EnumComboBox<Ptcl::TextureWrap> mWrapSComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mFilterComboBox{};
    SizedSpinBox<u8> mTexLodLevel{};

    QGroupBox mTexPatGroupBox{};
    QCheckBox mTexPatAnimCheckBox{};

    QComboBox mAnimModeComboBox{};

    SizedSpinBox<u16> mTexPatFreq{};
    SizedSpinBox<u16> mAnimFrameCountSpinBox{};

    TexturePreview mTexturePreview{};
    QPushButton mChangeTextureButton{};
    QLabel* mFramesWarningLabel{};

    SizedSpinBox<u8> mDivXSpinBox{};
    SizedSpinBox<u8> mDivYSpinBox{};
    SizedSpinBox<u16> mRepXSpinBox{};
    SizedSpinBox<u16> mRepYSpinBox{};

    QLabel* mDescriptionLabel{};

    FrameTimeline mFrameTimeline{};
    StartFrameList mStartFrameList{};
};


// ========================================================================== //


} // namespace PtclEditor
