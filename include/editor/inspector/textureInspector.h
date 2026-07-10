#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/frameTimeline.h"
#include "editor/components/sizedSpinBox.h"
#include "editor/components/startframelist.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QWidget>


namespace PtclEditor {


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
    void divisionsChanged(s32 divX, s32 divY);

protected:
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private:
    void updateLayoutCache();
    void drawTexture(QPainter& painter);
    void drawGrid(QPainter& painter);
    void drawSelection(QPainter& painter);
    void drawDimOverlay(QPainter& painter);

    void updateFrameRect();

private:
    static constexpr f32 UVScale = 2.0f;

    enum class DragMode {
        None,
        Division,
        Repetition
    };

private:
    QImage mTexture{};
    QPixmap mScaledPixmap{};

    QRect mTexRect{};
    QRect mFrameRect{};

    QSize mScaledSize{};
    qreal mScale{1.0f};

    s32 mDivX{1};
    s32 mDivY{1};

    s32 mRepX{1};
    s32 mRepY{1};

    s32 mPreviewRepX{1};
    s32 mPreviewRepY{1};

    s32 mPreviewDivX{1};
    s32 mPreviewDivY{1};

    QPoint mDragStart{};
    s32 mAccumulatedX{0};
    s32 mAccumulatedY{0};

    bool mDragging{false};

    DragMode mDragMode{DragMode::None};
};


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

private:
    EnumComboBox<Ptcl::TextureWrap> mWrapTComboBox{};
    EnumComboBox<Ptcl::TextureWrap> mWrapSComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mMagFilterComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mMinFilterComboBox{};
    EnumComboBox<Ptcl::TextureMipFilter> mMipFilterComboBox{};

    QGroupBox mTexPatGroupBox{};
    QCheckBox mTexPatAnimCheckBox{};

    QComboBox mAnimModeComboBox{};

    // SizedSpinBox<u16> mNumTexPat{};
    SizedSpinBox<u16> mTexPatFreq{};
    SizedSpinBox<u16> mAnimFrameCountSpinBox{};

    TextureRepetitionSelector mRepetitionSelector{};
    QPushButton mChangeTextureButton{};
    QLabel* mFramesWarningLabel{};

    FrameTimeline mFrameTimeline{};
    StartFrameList mStartFrameList{};
};


// ========================================================================== //


} // namespace PtclEditor
