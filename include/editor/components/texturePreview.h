#pragma once

#include "ptcl/ptclEnum.h"

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QGroupBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class TexturePreview final : public QWidget {
    Q_OBJECT
public:
    explicit TexturePreview(QWidget* parent = nullptr);

    void setSource(const QImage& image, s32 divX, s32 divY);
    void setRepetitions(s32 repX, s32 repY);
    void setDivisions(s32 divX, s32 divY);
    void setWrapModes(Ptcl::TextureWrap wrapT, Ptcl::TextureWrap wrapS);
    void setFilter(Ptcl::TextureFilter filter);

    void showFrameNumbers(bool show);

    QSize sizeHint() const override;
    s32 heightForWidth(s32 w) const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateLayoutCache();
    void rebuildTexture();

    void drawTexture(QPainter& painter);
    void drawFrameRegions(QPainter& painter);
    void drawFrameNumbers(QPainter& painter);

    QRect frameRegionRect(s32 frameIndex) const;

    void clampDivisions();

private:
    QImage mTexture{};
    QImage mRenderedTexture{};
    QImage mCheckerboard{};

    Ptcl::TextureWrap mWrapU{Ptcl::TextureWrap::MirroredRepeat};
    Ptcl::TextureWrap mWrapV{Ptcl::TextureWrap::MirroredRepeat};
    Ptcl::TextureFilter mFilter{Ptcl::TextureFilter::Nearest};

    QRect mTexRect{};
    f32 mScale{1.0f};

    s32 mDivX{1};
    s32 mDivY{1};
    s32 mRepX{1};
    s32 mRepY{1};

    bool mShowFrameNumbers{true};

    static constexpr s32 padding = 8;
};


// ========================================================================== //


} // namespace PtclEditor
