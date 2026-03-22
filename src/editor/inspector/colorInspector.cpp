#include "editor/inspector/colorInspector.h"

#include <QHBoxLayout>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


ColorInspector::ColorInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {
    // Color Behavior Type
    auto colorBehaviorLayout = new QHBoxLayout;
    mColorBehavior.addItem("Constant", QVariant::fromValue(Behavior::Constant));
    mColorBehavior.addItem("Random", QVariant::fromValue(Behavior::Random));
    mColorBehavior.addItem("Animation", QVariant::fromValue(Behavior::Animation));
    colorBehaviorLayout->addWidget(new QLabel("Color Behavior"));
    colorBehaviorLayout->addWidget(&mColorBehavior);
    colorBehaviorLayout->addStretch();

    // Primary Color Ui
    mPrimaryColorUi = new QWidget(this);
    auto* mPrimaryColorLayout = new QVBoxLayout(mPrimaryColorUi);
    mPrimaryColorLayout->addWidget(new QLabel("Primary Color"));
    mPrimaryColorLayout->addWidget(&mPrimaryColorWidget);

    // Random Color Ui
    mRandomColorUi = new QWidget(this);
    auto* mRandomColorLayout = new QVBoxLayout(mRandomColorUi);
    mRandomColorLayout->addWidget(new QLabel("Random Color A"));
    mRandomColorLayout->addWidget(&mRandomColorAWidget);
    mRandomColorLayout->addWidget(new QLabel("Random Color B"));
    mRandomColorLayout->addWidget(&mRandomColorBWidget);
    mRandomColorLayout->addWidget(new QLabel("Random Color C"));
    mRandomColorLayout->addWidget(&mRandomColorCWidget);

    // Color Repetition
    auto colorRepeatLayout = new QHBoxLayout;
    mRepetitionCountLabel.setText("Repetition Count");
    colorRepeatLayout->addWidget(&mRepetitionCountLabel);
    colorRepeatLayout->addWidget(&mColorNumRepeatSpinBox);
    colorRepeatLayout->addStretch();

    // Anim Color Ui
    mAnimColorUi = new QWidget(this);
    auto* mAnimColorLayout = new QVBoxLayout(mAnimColorUi);
    mAnimColorLayout->addWidget(new QLabel("Anim Start Color"));
    mAnimColorLayout->addWidget(&mStartColorWidget);
    mAnimColorLayout->addWidget(new QLabel("Anim Mid Color"));
    mAnimColorLayout->addWidget(&mMidColorWidget);
    mAnimColorLayout->addWidget(new QLabel("Anim End Color"));
    mAnimColorLayout->addWidget(&mEndColorWidget);
    mAnimColorLayout->addWidget(&mColorSections);
    mAnimColorLayout->addLayout(colorRepeatLayout);

    // Secondary Color
    mSecondaryColorWidget.enableAlpha(false);

    // Main Layout
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(colorBehaviorLayout);

    mainLayout->addWidget(mPrimaryColorUi);
    mainLayout->addWidget(mRandomColorUi);
    mainLayout->addWidget(mAnimColorUi);

    mainLayout->addWidget(new QLabel("Secondary Color"));
    mainLayout->addWidget(&mSecondaryColorWidget);
    mainLayout->addWidget(new QLabel("Color Calc Type"));
    mainLayout->addWidget(&mColorCalcTypeSpinBox);

    setLayout(mainLayout);
    setupConnections();
}

void ColorInspector::setupConnections() {
    // Behavior Type
    connect(&mColorBehavior, &QComboBox::currentIndexChanged, this, &ColorInspector::handleBehaviorChanged);

    // Color Sections
    connect(&mColorSections, &ColorGradientEditor::handleMoved, this, &ColorInspector::updateColorSection);

    // Color Repeat
    connect(&mColorNumRepeatSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Color Repetitions",
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
            "Set Color Calc Type",
            "SetColorCalcType",
            &Ptcl::Emitter::colorCalcType,
            &Ptcl::Emitter::setColorCalcType,
            type
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
}

void ColorInspector::populateProperties() {
    QSignalBlocker b1(mColorSections);
    QSignalBlocker b2(mColorNumRepeatSpinBox);
    QSignalBlocker b3(mColorCalcTypeSpinBox);
    QSignalBlocker b4(mPrimaryColorWidget);
    QSignalBlocker b5(mRandomColorAWidget);
    QSignalBlocker b6(mRandomColorBWidget);
    QSignalBlocker b7(mRandomColorCWidget);
    QSignalBlocker b8(mStartColorWidget);
    QSignalBlocker b9(mMidColorWidget);
    QSignalBlocker b10(mEndColorWidget);
    QSignalBlocker b11(mColorBehavior);

    mPrimaryColorWidget.setColor(mEmitter->primaryColor());

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
    mSecondaryColorWidget.setColor(mEmitter->secondaryColor());

    Behavior behavior = Behavior::Constant;
    if (mEmitter->isColorRandom()) {
        behavior = Behavior::Random;
        mPrimaryColorUi->setVisible(false);
        mRandomColorUi->setVisible(true);
        mAnimColorUi->setVisible(false);
    } else if (mEmitter->isColorAnimation()) {
        behavior = Behavior::Animation;
        mPrimaryColorUi->setVisible(false);
        mRandomColorUi->setVisible(false);
        mAnimColorUi->setVisible(true);
    } else {
        mPrimaryColorUi->setVisible(true);
        mRandomColorUi->setVisible(false);
        mAnimColorUi->setVisible(false);
    }

    mColorBehavior.setCurrentIndex(behaviorToIndex(behavior));
}

void ColorInspector::handleBehaviorChanged(s32 index) {
    auto behavior = behaviorFromIndex(index);

    bool isColorRandom;
    bool isColorAnim;
    QString label;

    switch (behavior) {
    case Behavior::Constant:
        label = "Set Color Behavior Constant";
        isColorRandom = false;
        isColorAnim = false;
        break;
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


// ========================================================================== //


} // namespace PtclEditor
