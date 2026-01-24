#pragma once

#include "ptcl/ptclEmitter.h"
#include "editor/enumComboBox.h"

#include <QWidget>


// ========================================================================== //


namespace Combiner {

enum class StageType {
    Color,
    Alpha
};

enum class Input {
    Constant,
    Primary,
    Texture,
    Previous
};

enum class Operand {
    Src,
    SrcR,
    OneMinusSrc
};

enum class Func {
    Replace,
    Modulate,
    Subtract,
    MultAdd
};

enum class Scale {
    One,
    Two
};

struct Stage {
    StageType type;
    std::array<Input, 3> inputs;
    std::array<Operand, 3> operands;
    Func func;
    Scale scale;
};

struct Config {
    std::array<Stage, 2> colorStage;
    std::array<Stage, 2> alphaStage;
};

static constexpr std::array<Config, 28> configurations = []() constexpr {
    using O = Operand;
    using I = Input;
    using F = Func;
    using S = Scale;
    using T = StageType;

    constexpr auto makeStage = [](T type, I in0, I in1, I in2, F func, O op0 = O::Src, O op1 = O::Src, O op2 = O::Src, S scale = S::One) constexpr {
        return Stage{ type, { in0, in1, in2 }, { op0, op1, op2 }, func, scale };
    };

    constexpr auto makeConfig = [](Stage c0, Stage a0, Stage c1, Stage a1) constexpr {
        return Config{ { c0, c1 }, { a0, a1 } };
    };

    return std::array<Config, 28> {{
        // Config 0
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 1
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 2
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd, O::Src, O::OneMinusSrc, O::Src),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 3
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate, O::Src, O::OneMinusSrc, O::Src),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 4
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 5
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 6
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Primary,  F::MultAdd),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 7
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 8
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 9
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd,  O::Src, O::OneMinusSrc, O::Src),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 10
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate, O::Src, O::OneMinusSrc, O::Src),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 11
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 12
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 13
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Primary,  F::MultAdd),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::Src, O::OneMinusSrc, O::Src, S::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 14
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcR, O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 15
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcR, O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 16
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcR,        O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd,  O::Src, O::OneMinusSrc, O::Src),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 17
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate, O::Src, O::OneMinusSrc, O::Src),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcR,        O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 18
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcR, O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 19
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcR, O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 20
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Primary,  F::MultAdd),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Primary,  I::Texture,  I::Previous, F::Modulate, O::Src, O::SrcR, O::Src),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 21
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcR, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 22
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcR, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 23
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcR, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd,  O::Src,  O::OneMinusSrc, O::Src),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 24
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Previous, F::Modulate, O::Src,  O::OneMinusSrc, O::Src),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcR, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Primary,  I::Texture,  I::Previous, F::MultAdd),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 25
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcR, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 26
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Primary,  I::Previous, I::Previous, F::Replace),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcR, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
        // Config 27
        makeConfig(
            /* Stage 0 Color */ makeStage(T::Color, I::Constant, I::Texture,  I::Primary,  F::MultAdd),
            /* Stage 0 Alpha */ makeStage(T::Alpha, I::Texture,  I::Primary,  I::Previous, F::Subtract, O::SrcR, O::OneMinusSrc, O::Src, Scale::Two),
            /* Stage 1 Color */ makeStage(T::Color, I::Previous, I::Previous, I::Previous, F::Replace),
            /* Stage 1 Alpha */ makeStage(T::Alpha, I::Previous, I::Previous, I::Previous, F::Replace)
        ),
    }};
}();

} // namespace Combiner


// ========================================================================== //


class CombinerStageWidget final : public QWidget {
    Q_OBJECT
public:
    explicit CombinerStageWidget(QWidget* parent = nullptr);
    void setStage(Combiner::Stage stage, s32 num);
    void setCombinerSrc(const Ptcl::TextureHandle* texture, const Ptcl::binColor3f* constant, const Ptcl::binColor4f* primary, const QImage* previous);

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
    const Ptcl::binColor3f* mConstantSrc{nullptr};
    const Ptcl::binColor4f* mPrimarySrc{nullptr};
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

    static constexpr QColor sEquationColor{200, 200, 200};
    static constexpr QColor sBackgroundColor{40, 40, 40};
    static constexpr QColor sBorderColor{70, 70, 70};

    static constexpr QColor sThumbBackgroundColor{60, 60, 60};
    static constexpr QColor sThumbBorderColor{90, 90, 90};

    static constexpr QColor sScaleLabelColor{60, 60, 60};
    static constexpr QColor sStageLabelColor{75, 75, 75};
};


// ========================================================================== //


class CombinerPreviewWidget final : public QWidget {
    Q_OBJECT
public:
    explicit CombinerPreviewWidget(QWidget* parent = nullptr);
    void setConfig(s32 configIdx);
    void setCombinerSrc(const Ptcl::TextureHandle* texture, const Ptcl::binColor3f* constant, const Ptcl::binColor4f* primary);

    void updateStages();

private:
    s32 mConfigIdx{-1};

    CombinerStageWidget mColorStage0{};
    CombinerStageWidget mColorStage1{};

    CombinerStageWidget mAlphaStage0{};
    CombinerStageWidget mAlphaStage1{};
};


// ========================================================================== //


class CombinerPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit CombinerPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::CombinerProperties& properties);
    void setCombinerSrc(const Ptcl::TextureHandle* texture, const Ptcl::binColor3f* constant, const Ptcl::binColor4f* primary);

    void updateCombinerPreview();

signals:
    void propertiesUpdated(const Ptcl::CombinerProperties& properties);

private:
    Ptcl::CombinerProperties mProps{};

    EnumComboBox<Ptcl::BlendFuncType> mBlendFuncComboBox{};
    EnumComboBox<Ptcl::DepthFuncType> mDepthFuncComboBox{};
    EnumComboBox<Ptcl::ColorCombinerFuncType> mCombinerFuncComboBox{};
    CombinerPreviewWidget mCombinerPreview{};
};


// ========================================================================== //
