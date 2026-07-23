#include "editor/inspector/colorInspector.h"

#include <QFormLayout>
#include <QLabel>


namespace PtclEditor {


// ==========================================================================//


static const std::array colorCalcTypeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::ColorCalcType>{ Ptcl::ColorCalcType::None,        "None",         "No color processing." },
    EnumOption<Ptcl::ColorCalcType>{ Ptcl::ColorCalcType::Pass1,       "Single Color", "Enables single pass color calculations. (Secondary blending color)." },
    EnumOption<Ptcl::ColorCalcType>{ Ptcl::ColorCalcType::Interpolate, "Multi Color",  "Enables multi-pass color calculations. (Randomized or Animated)." },
};

static const std::array behaviorOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<ColorInspector::Behavior>{ ColorInspector::Behavior::Random,    "Random",   "Each particle randomly picks one of three colors at spawn." },
    EnumOption<ColorInspector::Behavior>{ ColorInspector::Behavior::Animation, "Animated", "Color transitions through three colors over the particle's lifetime." },
};


// ==========================================================================//


ColorInspector::ColorInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    mMainLayout = new QFormLayout(this);
    mMainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Color
    addSectionHeader(mMainLayout, "Color", this);

    mColorCalcTypeSpinBox.setOptions(colorCalcTypeOptions);
    mColorCalcTypeSpinBox.setDescription("Controls how colors are applied to particles.");
    mMainLayout->addRow("Color Mode:", &mColorCalcTypeSpinBox);

    mPrimaryColorWidget.enableAlpha(false);
    mPrimaryColorWidget.setToolTip("The base color of each particle.");
    mMainLayout->addRow("Primary Color:", &mPrimaryColorWidget);

    mSecondaryColorWidget.setToolTip("A single color that can be used for blending in the texture combiner.");
    mMainLayout->addRow("Secondary Color:", &mSecondaryColorWidget);

    mColorBehavior.setOptions(behaviorOptions);
    mColorBehavior.setDescription("Determines the behavior used for multi-color calculations.");
    mMainLayout->addRow("Behavior:", &mColorBehavior);

    mRandomColorAWidget.setToolTip("First possible color. Each particle randomly picks A, B, or C at spawn.");
    mMainLayout->addRow("Random Color A:", &mRandomColorAWidget);

    mRandomColorBWidget.setToolTip("Second possible color. Each particle randomly picks A, B, or C at spawn.");
    mMainLayout->addRow("Random Color B:", &mRandomColorBWidget);

    mRandomColorCWidget.setToolTip("Third possible color. Each particle randomly picks A, B, or C at spawn.");
    mMainLayout->addRow("Random Color C:", &mRandomColorCWidget);

    mStartColorWidget.setToolTip("Color at the start of the animation (0% of particle lifetime).");
    mMainLayout->addRow("Anim Start Color:", &mStartColorWidget);

    mMidColorWidget.setToolTip("Color at the peak/middle of the animation.");
    mMainLayout->addRow("Anim Mid Color:", &mMidColorWidget);

    mEndColorWidget.setToolTip("Color at the end of the animation (100% of particle lifetime).");
    mMainLayout->addRow("Anim End Color:", &mEndColorWidget);

    mColorSections.setToolTip("Timing breakpoints (0-100% of particle lifetime) for color transitions.");
    mMainLayout->addRow("Color Timing:", &mColorSections);

    mColorNumRepeatSpinBox.setToolTip("Number of times the color animation repeats over the particle's lifetime.");
    mMainLayout->addRow("Repetition Count:", &mColorNumRepeatSpinBox);

    // Alpha
    addSectionHeader(mMainLayout, "Alpha", this);

    mGraphA.setLineColor(QColor{137, 214, 1});
    mGraphA.setTickStepSize(0.1f);
    mGraphA.setValueBounds(0.0f, 1.0f);
    mGraphA.setValueRange(0.0f, 1.0f);
    mGraphA.setVerticalAxisLabel("Alpha");
    mGraphA.setPosLabel("Life");
    mGraphA.setValLabel("Alpha");
    mMainLayout->addRow(&mGraphA);

    setupConnections();
}

void ColorInspector::setupConnections() {
    // Behavior Type
    connect(&mColorBehavior, &EnumComboBox<Behavior>::currentIndexChanged, this, [this]() {
        handleBehaviorChanged(mColorBehavior.currentEnum());
    });

    // Color Sections
    connect(&mColorSections, &ColorGradientEditor::handleMoved, this, &ColorInspector::updateColorSection);

    // Color Repeat
    connect(&mColorNumRepeatSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Number of Color Anim Repetitions",
            "SetColorRepetitions",
            &Ptcl::Emitter::colorNumRepeat,
            &Ptcl::Emitter::setColorNumRepeat,
            value
        );
    });

    // Color Calc Type
    connect(&mColorCalcTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mColorCalcTypeSpinBox.currentEnum();
        setEmitterProperty(
            "Set Color Mode",
            "SetColorCalcType",
            &Ptcl::Emitter::colorCalcType,
            &Ptcl::Emitter::setColorCalcType,
            type
        );
        updateVisibilityForCalcType(type);
    });

    // Secondary Color
    connect(&mSecondaryColorWidget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto& color = mSecondaryColorWidget.color();
        setEmitterProperty(
            "Set Secondary Color",
            "SetSecondaryColor",
            &Ptcl::Emitter::secondaryColor,
            &Ptcl::Emitter::setSecondaryColor,
            color
        );
    });

    // Random Color A
    connect(&mRandomColorAWidget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto& color = mRandomColorAWidget.color();
        setEmitterProperty(
            "Set Random Color A",
            "SetRandomColorA",
            &Ptcl::Emitter::randomColorA,
            &Ptcl::Emitter::setRandomColorA,
            color
         );
    });

    // Random Color B
    connect(&mRandomColorBWidget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto& color = mRandomColorBWidget.color();
        setEmitterProperty(
            "Set Random Color B",
            "SetRandomColorB",
            &Ptcl::Emitter::randomColorB,
            &Ptcl::Emitter::setRandomColorB,
            color
        );
    });

    // Random Color C
    connect(&mRandomColorCWidget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto& color = mRandomColorCWidget.color();
        setEmitterProperty(
            "Set Random Color C",
            "SetRandomColorC",
            &Ptcl::Emitter::randomColorC,
            &Ptcl::Emitter::setRandomColorC,
            color
        );
    });

    // Anim Start Color
    connect(&mStartColorWidget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto& color = mStartColorWidget.color();
        setEmitterProperty(
            "Set Start Color",
            "SetStartColor",
            &Ptcl::Emitter::startColor,
            &Ptcl::Emitter::setStartColor,
            color
        );
    });

    // Anim Mid Color
    connect(&mMidColorWidget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto& color = mMidColorWidget.color();
        setEmitterProperty(
            "Set Mid Color",
            "SetMidColor",
            &Ptcl::Emitter::midColor,
            &Ptcl::Emitter::setMidColor,
            color
        );
    });

    // Anim End Color
    connect(&mEndColorWidget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto& color = mEndColorWidget.color();
        setEmitterProperty(
            "Set End Color",
            "SetEndColor",
            &Ptcl::Emitter::endColor,
            &Ptcl::Emitter::setEndColor,
            color
        );
    });

    // Primary Color
    connect(&mPrimaryColorWidget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto& color = mPrimaryColorWidget.color();
        setEmitterProperty(
            "Set Primary Color",
            "SetPrimaryColor",
            &Ptcl::Emitter::primaryColor,
            &Ptcl::Emitter::setPrimaryColor,
            color
        );
    });

    // Alpha Animation
    connect(&mGraphA, &AnimGraph::pointEdited, this, [this](s32 pointIndex, const AnimGraph::GraphPoint& point) {
        updateAnimPoint(pointIndex, point);
    });
}

void ColorInspector::populateProperties() {
    QSignalBlocker b1(mColorSections);
    QSignalBlocker b2(mColorNumRepeatSpinBox);
    QSignalBlocker b3(mColorCalcTypeSpinBox);
    QSignalBlocker b4(mSecondaryColorWidget);
    QSignalBlocker b5(mRandomColorAWidget);
    QSignalBlocker b6(mRandomColorBWidget);
    QSignalBlocker b7(mRandomColorCWidget);
    QSignalBlocker b8(mStartColorWidget);
    QSignalBlocker b9(mMidColorWidget);
    QSignalBlocker b10(mEndColorWidget);
    QSignalBlocker b11(mColorBehavior);
    QSignalBlocker b12(mGraphA);
    QSignalBlocker b13(mPrimaryColorWidget);

    mSecondaryColorWidget.setColor(mEmitter->secondaryColor());

    mRandomColorAWidget.setColor(mEmitter->randomColorA());
    mRandomColorBWidget.setColor(mEmitter->randomColorB());
    mRandomColorCWidget.setColor(mEmitter->randomColorC());

    mStartColorWidget.setColor(mEmitter->startColor());
    mMidColorWidget.setColor(mEmitter->midColor());
    mEndColorWidget.setColor(mEmitter->endColor());

    mColorSections.setTimings(
        mEmitter->colorSection1(),
        mEmitter->colorSection2(),
        mEmitter->colorSection3()
    );

    const auto& startColor = mEmitter->startColor();
    mColorSections.setInitialColor(QColor::fromRgbF(startColor.r(), startColor.g(), startColor.b()));

    const auto& midColor = mEmitter->midColor();
    mColorSections.setPeakColor(QColor::fromRgbF(midColor.r(), midColor.g(), midColor.b()));

    const auto& endColor = mEmitter->endColor();
    mColorSections.setEndColor(QColor::fromRgbF(endColor.r(), endColor.g(), endColor.b()));

    mColorSections.setRepetitionCount(mEmitter->colorNumRepeat());
    mColorNumRepeatSpinBox.setValue(mEmitter->colorNumRepeat());
    mColorCalcTypeSpinBox.setCurrentEnum(mEmitter->colorCalcType());
    mPrimaryColorWidget.setColor(mEmitter->primaryColor());

    Behavior behavior = Behavior::Random;

    if (mEmitter->isColorAnimation()) {
        behavior = Behavior::Animation;
    }

    mColorBehavior.setCurrentEnum(behavior);
    updateVisibilityForCalcType(mEmitter->colorCalcType());

    // Alpha Animation
    const bool emitterChanged = (mEmitter != mLastEmitter);
    mLastEmitter = mEmitter;

    const auto& anim = mEmitter->alphaAnim();

    const f32 p0 = anim.initAlpha;
    const f32 p1 = p0 + anim.diffAlpha21;
    const f32 p2 = p1;
    const f32 p3 = p2 + anim.diffAlpha32;

    const bool fullLife = anim.alphaSection2 == 128;

    const f32 sec1 = static_cast<f32>(anim.alphaSection1);
    const f32 sec2 = fullLife ? 100.0f : static_cast<f32>(anim.alphaSection2);

    AnimGraph::PointList points = {
        { 0.0f, p0, AnimGraph::HandleType::Locked },
        { sec1, p1, AnimGraph::HandleType::HoldStart },
        { sec2, p2, AnimGraph::HandleType::HoldEnd },
        { 100.0f, p3, AnimGraph::HandleType::Locked }
    };
    mGraphA.setControlPoints(points);

    if (emitterChanged) {
        mGraphA.zoomToFit();
    }

    update();
}

void ColorInspector::handleBehaviorChanged(Behavior behavior) {
    bool isColorRandom;
    bool isColorAnim;
    QString label;

    switch (behavior) {
    case Behavior::Random:
        label = "Set Color Behavior Random";
        isColorRandom = true;
        isColorAnim = false;
        break;
    case Behavior::Animation:
        label = "Set Color Behavior Animation";
        isColorRandom = false;
        isColorAnim = true;
        break;
    }

    bool showRandom = (behavior == Behavior::Random);
    bool showAnim = (behavior == Behavior::Animation);

    mMainLayout->setRowVisible(&mRandomColorAWidget, showRandom);
    mMainLayout->setRowVisible(&mRandomColorBWidget, showRandom);
    mMainLayout->setRowVisible(&mRandomColorCWidget, showRandom);
    mMainLayout->setRowVisible(&mStartColorWidget, showAnim);
    mMainLayout->setRowVisible(&mMidColorWidget, showAnim);
    mMainLayout->setRowVisible(&mEndColorWidget, showAnim);
    mMainLayout->setRowVisible(&mColorSections, showAnim);
    mMainLayout->setRowVisible(&mColorNumRepeatSpinBox, showAnim);

    mDocument->undoStack()->beginMacro(formatHistoryLabel(label));

    setEmitterProperty(
        "Set Color Random",
        "SetColorRandom",
        &Ptcl::Emitter::isColorRandom,
        &Ptcl::Emitter::setIsColorRandom,
        isColorRandom
    );

    setEmitterProperty(
        "Set Color Anim",
        "SetColorAnim",
        &Ptcl::Emitter::isColorAnimation,
        &Ptcl::Emitter::setIsColorAnimation,
        isColorAnim
    );

    mDocument->undoStack()->endMacro();
}

void ColorInspector::updateVisibilityForCalcType(Ptcl::ColorCalcType type) {
    const bool isMultiColor = (type == Ptcl::ColorCalcType::Interpolate);
    const bool isSingleColor = (type == Ptcl::ColorCalcType::Pass1);
    mMainLayout->setRowVisible(&mColorBehavior, isMultiColor);
    mMainLayout->setRowVisible(&mSecondaryColorWidget, isSingleColor);

    if (isMultiColor) {
        auto behavior = mColorBehavior.currentEnum();
        bool showRandom = (behavior == Behavior::Random);
        bool showAnim = (behavior == Behavior::Animation);

        mMainLayout->setRowVisible(&mRandomColorAWidget, showRandom);
        mMainLayout->setRowVisible(&mRandomColorBWidget, showRandom);
        mMainLayout->setRowVisible(&mRandomColorCWidget, showRandom);
        mMainLayout->setRowVisible(&mStartColorWidget, showAnim);
        mMainLayout->setRowVisible(&mMidColorWidget, showAnim);
        mMainLayout->setRowVisible(&mEndColorWidget, showAnim);
        mMainLayout->setRowVisible(&mColorSections, showAnim);
        mMainLayout->setRowVisible(&mColorNumRepeatSpinBox, showAnim);
    } else {
        mMainLayout->setRowVisible(&mRandomColorAWidget, false);
        mMainLayout->setRowVisible(&mRandomColorBWidget, false);
        mMainLayout->setRowVisible(&mRandomColorCWidget, false);
        mMainLayout->setRowVisible(&mStartColorWidget, false);
        mMainLayout->setRowVisible(&mMidColorWidget, false);
        mMainLayout->setRowVisible(&mEndColorWidget, false);
        mMainLayout->setRowVisible(&mColorSections, false);
        mMainLayout->setRowVisible(&mColorNumRepeatSpinBox, false);
    }
}

void ColorInspector::updateColorSection(ColorGradientEditor::HandleType handleType) {
    switch (handleType) {
    case ColorGradientEditor::HandleType::InCompletedHandle: {
        setEmitterProperty(
            "Set Color Anim In Timing",
            "SetColorAnimSection1",
            &Ptcl::Emitter::colorSection1,
            &Ptcl::Emitter::setColorSection1,
            mColorSections.inCompletedTiming()
        );
    }
    break;
    case ColorGradientEditor::HandleType::PeakHandle: {
        setEmitterProperty(
            "Set Color Anim Peak Timing",
            "SetColorAnimSection2",
            &Ptcl::Emitter::colorSection2,
            &Ptcl::Emitter::setColorSection2,
            mColorSections.peakTiming()
        );
    }
    break;
    case ColorGradientEditor::HandleType::OutStartHandle: {
        setEmitterProperty(
            "Set Color Anim Out Timing",
            "SetColorAnimSection3",
            &Ptcl::Emitter::colorSection3,
            &Ptcl::Emitter::setColorSection3,
            mColorSections.outStartTiming()
        );
    }
    break;
    }
}

void ColorInspector::updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point) {
    auto anim = mEmitter->alphaAnim();

    const f32 oldP0 = anim.initAlpha;
    const f32 oldP1 = oldP0 + anim.diffAlpha21;
    const f32 oldP2 = oldP1;
    const f32 oldP3 = oldP2 + anim.diffAlpha32;

    auto updateAlphaSection = [&](s32* section) {
        anim.diffAlpha21 = point.value - oldP0;
        anim.diffAlpha32 = oldP3 - point.value;

        const f32 sec2 = mGraphA.getPoints()[2].position;

        *section = (std::abs(sec2 - 100.0f) < std::numeric_limits<f32>::epsilon()) ? 128 : static_cast<s32>(point.position);
    };

    switch (pointIndex) {
    case 0:
        anim.initAlpha = point.value;
        anim.diffAlpha21 = oldP1 - point.value;
        break;
    case 1:
        updateAlphaSection(&anim.alphaSection1);
        {
            const f32 sec2Pos = mGraphA.getPoints()[2].position;
            anim.alphaSection2 = (std::abs(sec2Pos - 100.0f) < std::numeric_limits<f32>::epsilon())
                ? 128 : static_cast<s32>(sec2Pos);
        }
        break;
    case 2:
        updateAlphaSection(&anim.alphaSection2);
        anim.alphaSection1 = static_cast<s32>(mGraphA.getPoints()[1].position);
        break;
    case 3:
        anim.diffAlpha32 = point.value - oldP1;
        break;
    }

    const f32 newP0 = anim.initAlpha;
    const f32 newP1 = newP0 + anim.diffAlpha21;
    anim.isFlatStart = (newP0 == newP1);

    QString handleName;
    switch (pointIndex) {
    case 0: handleName = "Start"; break;
    case 1: handleName = "Section1"; break;
    case 2: handleName = "Section2"; break;
    case 3: handleName = "End"; break;
    }

    const auto label = QString("Move Alpha %1").arg(handleName);
    const auto key = QString("AlphaGraph_%1").arg(pointIndex);

    setEmitterProperty(
        label,
        key,
        &Ptcl::Emitter::alphaAnim,
        &Ptcl::Emitter::setAlphaAnim,
        anim
    );
}


// ==========================================================================//


} // namespace PtclEditor
