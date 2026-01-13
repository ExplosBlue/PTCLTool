#include "editor/colorPropertiesWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>


// ========================================================================== //


ColorPropertiesWidget::ColorPropertiesWidget(QWidget* parent) :
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
    for (int i = 0; i < mColorWidgets.size(); ++i) {
        colorsLayout->addWidget(&mColorWidgets[i]);
        mColorWidgets[i].setProperty("colorIndex", i);
        connect(&mColorWidgets[i], &RGBAColorWidget::colorChanged, this, &ColorPropertiesWidget::handleColorChanged);
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
    connect(&mColorNumRepeatSpinBox, &SizedSpinBoxBase::valueChanged, this, [=, this](int value) {
        mColorSections.setRepetitionCount(value);
        mProps.colorNumRepeat = value;
    });

    // Main Layout
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(colorBehaviorLayout);
    mainLayout->addLayout(colorLabelLayout);
    mainLayout->addLayout(colorsLayout);
    mainLayout->addWidget(&mColorSections);
    mainLayout->addLayout(colorRepeatLayout);

    setLayout(mainLayout);
}

void ColorPropertiesWidget::setProperties(const Ptcl::ColorProperties& properties) {
    mProps = properties;
    populateWidgets();
}

void ColorPropertiesWidget::populateWidgets() {
    QSignalBlocker b1(mColorSections);
    QSignalBlocker b2(mColorNumRepeatSpinBox);

    QSignalBlocker bColor0(mColorWidgets[0]);
    QSignalBlocker bColor1(mColorWidgets[1]);
    QSignalBlocker bColor2(mColorWidgets[2]);

    for (int i = 0; i < mColorWidgets.size(); ++i) {
        mColorWidgets[i].setColor(mProps.colors[i]);
    }

    mColorSections.setTimings(
        mProps.colorSection1,
        mProps.colorSection2,
        mProps.colorSection3
    );

    const auto& colors = mProps.colors;
    mColorSections.setInitialColor(QColor::fromRgbF(colors[0].r, colors[0].g, colors[0].b));
    mColorSections.setPeakColor(QColor::fromRgbF(colors[1].r, colors[1].g, colors[1].b));
    mColorSections.setEndColor(QColor::fromRgbF(colors[2].r, colors[2].g, colors[2].b));

    mColorSections.setRepetitionCount(mProps.colorNumRepeat);
    mColorNumRepeatSpinBox.setValue(mProps.colorNumRepeat);

    updateUiFromFlags();
}

void ColorPropertiesWidget::updateUiFromFlags() {

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

void ColorPropertiesWidget::applyBehaviorToUI(Behavior behavior) {
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

void ColorPropertiesWidget::showColorWidgets(std::array<bool, 3> visibility) {
    for (int i = 0; i < mColorWidgets.size(); ++i) {
        mColorWidgets[i].setVisible(visibility[i]);
    }
}

void ColorPropertiesWidget::setColorLabels(const std::array<QString, 3>& labels) {
    for (int i = 0; i < mColorLabels.size(); ++i) {
        mColorLabels[i].setText(labels[i]);
        mColorLabels[i].setVisible(!labels[i].isEmpty());
    }
}

void ColorPropertiesWidget::handleBehaviorChanged(int index) {
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

void ColorPropertiesWidget::handleColorChanged() {
    auto* widget = qobject_cast<RGBAColorWidget*>(sender());
    if (!widget) {
        return;
    }

    int index = widget->property("colorIndex").toInt();
    if (index < 0 || index >= mColorWidgets.size()) {
        return;
    }

    const auto& color = widget->color();
    mProps.colors[index] = color;

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

void ColorPropertiesWidget::updateColorSection(ColorGradientEditor::HandleType handleType) {
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
