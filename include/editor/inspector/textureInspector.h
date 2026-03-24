#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/sizedSpinBox.h"
#include "editor/components/thumbnailWidget.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QGroupBox>
#include <QTableWidget>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class TextureDivisionSelector final : public QWidget {
    Q_OBJECT
public:
    explicit TextureDivisionSelector(QWidget* parent = nullptr);

    void setDivisions(s32 x, s32 y);
    void setTexture(const QImage& image);

    QSize sizeHint() const override;

signals:
    void divisionsChanged(s32 x, s32 y);

protected:
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private:
    void updateLayoutCache();

private:
    QImage mTexture{};
    QPixmap mScaledPixmap{};

    QRect mTexRect{};
    QSize mScaledSize{};
    qreal mScale{1.0f};

    s32 mDivX{1};
    s32 mDivY{1};

    s32 mPreviewDivX{1};
    s32 mPreviewDivY{1};

    QPoint mDragStart{};
    bool mDragging{false};
};


// ========================================================================== //


class TextureRepetitionSelector final : public QWidget {
    Q_OBJECT
public:
    explicit TextureRepetitionSelector(QWidget* parent = nullptr);

    void setSource(const QImage& image, s32 divX, s32 divY);
    void setRepetitions(s32 repX, s32 repY);

    QSize sizeHint() const override;

signals:
    void repetitionsChanged(s32 repX, s32 repY);

protected:
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private:
    void updateLayoutCache();

private:
    QImage mTexture{};
    QPixmap mScaledPixmap{};

    QRect mTexRect{};
    QSize mScaledSize{};
    qreal mScale{1.0f};

    s32 mDivX{1};
    s32 mDivY{1};

    s32 mRepX{1};
    s32 mRepY{1};

    s32 mPreviewRepX{1};
    s32 mPreviewRepY{1};

    QPoint mDragStart{};
    bool mDragging{false};
};


// ========================================================================== //


class TextureInspector final : public InspectorWidgetBase {
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
    explicit TextureInspector(QWidget* parent = nullptr);

private slots:
    void changeTexture();

private:
    void populateProperties() final;
    void setupConnections();

    void updateTexPatTblColumns();

    std::optional<Math::Vector2f> calcFrameUVOffset(s32 frame) const;
    QImage getFrameTexture(s32 frame) const;
    QImage applyUVRepetition(const QImage& image, f32 repeatX, f32 repeatY) const;
    QPixmap createFramePreview(s32 frame) const;

    s32 maxFrameCount() const;

private:
    ThumbnailWidget mTexturePreview{};

    EnumComboBox<Ptcl::TextureWrap> mWrapTComboBox{};
    EnumComboBox<Ptcl::TextureWrap> mWrapSComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mMagFilterComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mMinFilterComboBox{};
    EnumComboBox<Ptcl::TextureMipFilter> mMipFilterComboBox{};

    QGroupBox mTexPatGroupBox{};

    QComboBox mAnimModeComboBox{};

    SizedSpinBox<u16> mNumTexPat{};
    SizedSpinBox<u16> mTexPatFreq{};
    SizedSpinBox<u16> mTexPatTblUse{};

    TextureDivisionSelector mDivisionSelector{};
    TextureRepetitionSelector mRepetitionSelector{};

    // SizedSpinBox<u32> mTexRepetitionsX{};
    // SizedSpinBox<u32> mTexRepetitionsY{};

    QTableWidget mTexPatTbl{1, 16, this};
};


// ========================================================================== //


} // namespace PtclEditor
