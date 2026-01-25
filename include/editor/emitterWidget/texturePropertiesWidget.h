#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/sizedSpinBox.h"
#include "editor/components/thumbnailWidget.h"
#include "editor/emitterWidget/emitterWidget.h"

#include "ptcl/ptcl.h"
#include "ptcl/ptclEmitter.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QTableWidget>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidget::TexturePropertiesWidget final : public QWidget {
    Q_OBJECT

public:
    enum class AnimMode {
        FixedSpeed = 0,
        FitToLife  = 1
    };

    inline s32 animModeToFreq(AnimMode mode) {
        return static_cast<s32>(mode);
    }

    inline AnimMode freqToAnimMode(s32 value) {
        return (value > 0) ? AnimMode::FixedSpeed : AnimMode::FitToLife;
    }

public:
    explicit TexturePropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::TextureProperties& properties, const std::shared_ptr<Ptcl::Texture>& texture);

    void populateWidgets();
    void setTextureList(const Ptcl::TextureList* textureList);

signals:
    void propertiesUpdated(const Ptcl::Emitter::TextureProperties& properties);
    void textureUpdated(const std::shared_ptr<Ptcl::Texture>& oldTexture, const std::shared_ptr<Ptcl::Texture>& newTexture);

private slots:
    void changeTexture();

private:
    void updateTextureDetails();
    void updateTexPatTblColumns();
    void updateUVScale();

    std::optional<Math::Vector2f> calcFrameUVOffset(s32 frame) const;
    QImage getFrameTexture(s32 frame) const;
    QImage applyUVRepetition(const QImage& image, f32 repeatX, f32 repeatY) const;
    QPixmap createFramePreview(s32 frame) const;

    s32 maxFrameCount() const;

private:
    Ptcl::Emitter::TextureProperties mProps{};
    std::shared_ptr<Ptcl::Texture> mTexture{};

    const Ptcl::TextureList* mTextureList{nullptr};

    ThumbnailWidget mTexturePreview{};

    EnumComboBox<Ptcl::TextureWrap> mWrapTComboBox{};
    EnumComboBox<Ptcl::TextureWrap> mWrapSComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mMagFilterComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mMinFilterComboBox{};
    EnumComboBox<Ptcl::TextureMipFilter> mMipFilterComboBox{};

    QGroupBox mTexPatGroupBox{};

    QComboBox mAnimModeComboBox{};

    SizedSpinBox<u16> mNumTexPat{};
    SizedSpinBox<u8> mTexDivX{};
    SizedSpinBox<u8> mTexDivY{};
    SizedSpinBox<u16> mTexPatFreq{};
    SizedSpinBox<u16> mTexPatTblUse{};

    SizedSpinBox<u32> mTexRepetitionsX{};
    SizedSpinBox<u32> mTexRepetitionsY{};

    QTableWidget mTexPatTbl{1, 16, this};
};


// ========================================================================== //


} // namespace PtclEditor
