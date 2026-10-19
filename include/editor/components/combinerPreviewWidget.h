#pragma once

#include "gfx/color.h"
#include "ptcl/renderer/combiner.h"

#include "ptcl/ptclTexture.h"

#include <QWidget>
#include <QCheckBox>

#include <array>


namespace PtclTool {

namespace Combiner = Ptcl::Render::Combiner;


// ========================================================================== //





// ========================================================================== //


class CombinerStageWidget final : public QWidget {
    Q_OBJECT
public:
    explicit CombinerStageWidget(QWidget* parent = nullptr);
    void setStage(Combiner::Stage stage, s32 num);
    void setCombinerSrc(const Ptcl::TextureHandle* texture, const Gfx::Color* constant, const Gfx::Color* primary, const QImage* previous);

    const QImage& getStageOutput() const;

    void updateStage();

protected:
    void paintEvent(QPaintEvent* event) final;
    QSize sizeHint() const final;

private:
    void drawBackground(QPainter& painter);
    void drawInput(QPainter& painter, const QRect& drawRect, s32 inputIdx);
    void drawEquation(QPainter& painter, const QRect& drawRect);
    void drawReplace(QPainter& painter, const QRect& drawRect);
    void drawMultAdd(QPainter& painter, const QRect& drawRect);
    void drawBinaryOp(QPainter& painter, const QRect& drawRect, const QString& operand);
    void drawParen(QPainter& painter, const QRect& drawRect, bool isLeft);
    void drawScaleLabel(QPainter& painter, const QRect& drawRect);
    void drawStageLabel(QPainter& painter, const QRect& drawRect);

    QImage colorToImage(const QColor& color) const;
    QImage visualizeColorOrAlpha(const QImage& src) const;
    QImage applyOperandTransform(const QImage& src, Combiner::Operand operand) const;

    QImage getTransformedInput(s32 inputIdx) const;
    QImage scaleForThumbnail(const QImage& src) const;

    s32 calcEquationWidth() const;
    constexpr s32 calcEquationHeight() const;

    QFont equationFont() const;
    QFont scaleFont() const;

    void updateStageOutput();

private:
    Combiner::Stage mStage{};
    s32 mStageNum{0};

    const Ptcl::TextureHandle* mTextureSrc{nullptr};
    const Gfx::Color* mConstantSrc{nullptr};
    const Gfx::Color* mPrimarySrc{nullptr};
    const QImage* mPrevSrc{nullptr};

    QImage mOutputImage{};

    static constexpr s32 sPadding = 16;

    static constexpr s32 sBorderRadius = 6;
    static constexpr s32 sMargin = 1;

    static constexpr s32 sThumbSize = 32;
    static constexpr s32 sThumbPadding = 3;
    static constexpr s32 sThumbRadius = 4;

    static constexpr s32 sLabelHeight = 14;
    static constexpr s32 sLabelGap = 8;

    static constexpr s32 sOperatorWidth = 20;
    static constexpr s32 sOperatorGap = 8;

    static constexpr s32 sParenPad = 4;
    static constexpr s32 sParenW = sThumbSize / 3;
    static constexpr s32 sParenH = sThumbSize + (sParenPad * 2);


};


// ========================================================================== //


class CombinerPreviewWidget final : public QWidget {
    Q_OBJECT
public:
    explicit CombinerPreviewWidget(QWidget* parent = nullptr);
    void setConfig(s32 configIdx);
    void setCombinerSrc(const Ptcl::TextureHandle* texture, const Gfx::Color* constant, const Gfx::Color* primary);

    void updateStages();

private:
    s32 mConfigIdx{-1};

    CombinerStageWidget mColorStage0{};
    CombinerStageWidget mColorStage1{};

    CombinerStageWidget mAlphaStage0{};
    CombinerStageWidget mAlphaStage1{};
};


// ========================================================================== //


} // namespace PtclTool
