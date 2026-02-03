#include "editor/emitterWidget/colorPropertiesWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


EmitterWidget::ColorPropertiesWidget::ColorPropertiesWidget(QWidget* parent) :
    QWidget{parent} {
    // Color Behavior Type
    auto colorBehaviorLayout = new QHBoxLayout;
    mColorBehavior.addItem("Constant", QVariant::fromValue(Behavior::Constant));
    mColorBehavior.addItem("Random", QVariant::fromValue(Behavior::Random));
    mColorBehavior.addItem("Animation", QVariant::fromValue(Behavior::Animation));
    colorBehaviorLayout->addWidget(new QLabel("Color Behavior"));
    colorBehaviorLayout->addWidget(&mColorBehavior);
    colorBehaviorLayout->addStretch();
    connect(&mColorBehavior, &QComboBox::currentIndexChanged, this, &ColorPropertiesWidget::handleBehaviorChanged);

    // Color Properties
    auto colorLabelLayout = new QHBoxLayout;
    auto colorsLayout = new QHBoxLayout;
    for (s32 i = 0; i < mColor0Widgets.size(); ++i) {
        colorsLayout->addWidget(&mColor0Widgets[i]);
        mColor0Widgets[i].setProperty("colorIndex", i);
        connect(&mColor0Widgets[i], &RGBAColorWidget::colorChanged, this, &ColorPropertiesWidget::handleColorChanged);
        colorLabelLayout->addWidget(&mColorLabels[i]);
    }

    // Color Sections
    connect(&mColorSections, &ColorGradientEditor::handleMoved, this, &ColorPropertiesWidget::updateColorSection);

    // Color Repeat
    auto colorRepeatLayout = new QHBoxLayout;
    mRepetitionCountLabel.setText("Repetition Count");
    colorRepeatLayout->addWidget(&mRepetitionCountLabel);
    colorRepeatLayout->addWidget(&mColorNumRepeatSpinBox);
    colorRepeatLayout->addStretch();
    connect(&mColorNumRepeatSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](s32 value) {
        mColorSections.setRepetitionCount(value);
        mProps.colorNumRepeat = value;
    });

    // Color Calc Type
    connect(&mColorCalcTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.colorCalcType = mColorCalcTypeSpinBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    // Color1
    mColor1Widget.enableAlpha(false);
    connect(&mColor1Widget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto& color = mColor1Widget.color();

        Ptcl::binColor3f newColor{
            color.r * 255.0f,
            color.g * 255.0f,
            color.b * 255.0f
        };

        mProps.color1 = newColor;
        emit propertiesUpdated(mProps);
    });

    // Main Layout
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(colorBehaviorLayout);
    mainLayout->addLayout(colorLabelLayout);
    mainLayout->addLayout(colorsLayout);
    mainLayout->addWidget(&mColorSections);
    mainLayout->addLayout(colorRepeatLayout);
    mainLayout->addWidget(new QLabel("Color1:"));
    mainLayout->addWidget(&mColor1Widget);
    mainLayout->addWidget(new QLabel("Calc Type:"));
    mainLayout->addWidget(&mColorCalcTypeSpinBox);

    setLayout(mainLayout);
}

void EmitterWidget::ColorPropertiesWidget::setProperties(const Ptcl::Emitter::ColorProperties& properties) {
    mProps = properties;
    populateWidgets();
}

void EmitterWidget::ColorPropertiesWidget::populateWidgets() {
    QSignalBlocker b1(mColorSections);
    QSignalBlocker b2(mColorNumRepeatSpinBox);
    QSignalBlocker b3(mColorCalcTypeSpinBox);
    QSignalBlocker b4(mColor1Widget);

    QSignalBlocker bColor0(mColor0Widgets[0]);
    QSignalBlocker bColor1(mColor0Widgets[1]);
    QSignalBlocker bColor2(mColor0Widgets[2]);

    for (s32 i = 0; i < mColor0Widgets.size(); ++i) {
        mColor0Widgets[i].setColor(mProps.color0[i]);
    }

    mColorSections.setTimings(
        mProps.colorSection1,
        mProps.colorSection2,
        mProps.colorSection3
    );

    const auto& colors = mProps.color0;
    mColorSections.setInitialColor(QColor::fromRgbF(colors[0].r, colors[0].g, colors[0].b));
    mColorSections.setPeakColor(QColor::fromRgbF(colors[1].r, colors[1].g, colors[1].b));
    mColorSections.setEndColor(QColor::fromRgbF(colors[2].r, colors[2].g, colors[2].b));

    mColorSections.setRepetitionCount(mProps.colorNumRepeat);
    mColorNumRepeatSpinBox.setValue(mProps.colorNumRepeat);
    mColorCalcTypeSpinBox.setCurrentEnum(mProps.colorCalcType);

    Ptcl::binColor4f color1{
        std::clamp(mProps.color1.r / 255.0f, 0.0f, 1.0f),
        std::clamp(mProps.color1.g / 255.0f, 0.0f, 1.0f),
        std::clamp(mProps.color1.b / 255.0f, 0.0f, 1.0f),
        1.0f
    };
    mColor1Widget.setColor(color1);

    updateUiFromFlags();
}

void EmitterWidget::ColorPropertiesWidget::updateUiFromFlags() {

    Behavior behavior = Behavior::Constant;
    if (mProps.colorRandom) {
        behavior = Behavior::Random;
    } else if (mProps.colorAnimation) {
        behavior = Behavior::Animation;
    }

    QSignalBlocker blocker(mColorBehavior);
    mColorBehavior.setCurrentIndex(behaviorToIndex(behavior));
    applyBehaviorToUI(behavior);
}

void EmitterWidget::ColorPropertiesWidget::applyBehaviorToUI(Behavior behavior) {
    switch (behavior) {
    case Behavior::Constant:
        showColorWidgets({true, false, false});
        setColorLabels({"Constant Color", "", ""});
        mColorSections.setVisible(false);
        break;
    case Behavior::Random:
        showColorWidgets({true, true, true});
        setColorLabels({"Random Color 1", "Random Color 2", "Random Color 3"});
        mColorSections.setVisible(false);
        break;
    case Behavior::Animation:
        showColorWidgets({true, true, true});
        setColorLabels({"Initial Color", "Peak Color", "Out Color"});
        mColorSections.setVisible(true);
        break;
    }

    const bool showRepeat = (behavior == Behavior::Animation);
    mColorNumRepeatSpinBox.setVisible(showRepeat);
    mRepetitionCountLabel.setVisible(showRepeat);
}

void EmitterWidget::ColorPropertiesWidget::showColorWidgets(std::array<bool, 3> visibility) {
    for (s32 i = 0; i < mColor0Widgets.size(); ++i) {
        mColor0Widgets[i].setVisible(visibility[i]);
    }
}

void EmitterWidget::ColorPropertiesWidget::setColorLabels(const std::array<QString, 3>& labels) {
    for (s32 i = 0; i < mColorLabels.size(); ++i) {
        mColorLabels[i].setText(labels[i]);
        mColorLabels[i].setVisible(!labels[i].isEmpty());
    }
}

void EmitterWidget::ColorPropertiesWidget::handleBehaviorChanged(s32 index) {
    auto behavior = behaviorFromIndex(index);

    applyBehaviorToUI(behavior);

    switch (behavior) {
    case Behavior::Constant:
        mProps.colorRandom = false;
        mProps.colorAnimation = false;
        break;
    case Behavior::Random:
        mProps.colorRandom = true;
        mProps.colorAnimation = false;
        break;
    case Behavior::Animation:
        mProps.colorRandom = false;
        mProps.colorAnimation = true;
        break;
    }

    emit propertiesUpdated(mProps);
}

void EmitterWidget::ColorPropertiesWidget::handleColorChanged() {
    auto* widget = qobject_cast<RGBAColorWidget*>(sender());
    if (!widget) {
        return;
    }

    s32 index = widget->property("colorIndex").toInt();
    if (index < 0 || index >= mColor0Widgets.size()) {
        return;
    }

    const auto& color = widget->color();
    mProps.color0[index] = color;

    QColor qcolor = QColor::fromRgbF(color.r, color.g, color.b);
    if (index == 0) {
        mColorSections.setInitialColor(qcolor);
    } else if (index == 1) {
        mColorSections.setPeakColor(qcolor);
    } else if (index == 2) {
        mColorSections.setEndColor(qcolor);
    }

    emit propertiesUpdated(mProps);
}

void EmitterWidget::ColorPropertiesWidget::updateColorSection(ColorGradientEditor::HandleType handleType) {
    switch (handleType) {
    case ColorGradientEditor::HandleType::InCompletedHandle:
        mProps.colorSection1 = mColorSections.inCompletedTiming();
        break;
    case ColorGradientEditor::HandleType::PeakHandle:
        mProps.colorSection2 = mColorSections.peakTiming();
        break;
    case ColorGradientEditor::HandleType::OutStartHandle:
        mProps.colorSection3 = mColorSections.outStartTiming();
        break;
    }

    emit propertiesUpdated(mProps);
}


// ========================================================================== //


} // namespace PtclEditor
