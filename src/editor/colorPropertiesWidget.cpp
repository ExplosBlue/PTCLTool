#include "editor/colorPropertiesWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>


// ========================================================================== //


ColorPropertiesWidget::ColorPropertiesWidget(QWidget* parent) :
    QWidget{parent}, mIsPopulating{false} {
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

        // Label
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
        if (!mEmitterPtr) {
            return;
        }
        mEmitterPtr->setColorNumRepeat(value);
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

void ColorPropertiesWidget::setEmitter(Ptcl::Emitter* emitter) {
    mEmitterPtr = emitter;
    populateWidgets();
}

void ColorPropertiesWidget::populateWidgets() {
    if (!mEmitterPtr) {
        return;
    }

    mIsPopulating = true;

    for (int i = 0; i < mColorWidgets.size(); ++i) {
        mColorWidgets[i].setColor(mEmitterPtr->colors()[i]);
    }

    mColorSections.setTimings(
        mEmitterPtr->colorSection1(),
        mEmitterPtr->colorSection2(),
        mEmitterPtr->colorSection3()
    );

    const auto& colors = mEmitterPtr->colors();
    mColorSections.setInitialColor(QColor::fromRgbF(colors[0].r, colors[0].g, colors[0].b));
    mColorSections.setPeakColor(QColor::fromRgbF(colors[1].r, colors[1].g, colors[1].b));
    mColorSections.setEndColor(QColor::fromRgbF(colors[2].r, colors[2].g, colors[2].b));

    mColorSections.setRepetitionCount(mEmitterPtr->colorNumRepeat());
    mColorNumRepeatSpinBox.setValue(mEmitterPtr->colorNumRepeat());

    updateUiFromFlags();

    mIsPopulating = false;
}

void ColorPropertiesWidget::updateUiFromFlags() {
    if (!mEmitterPtr) {
        return;
    }

    Behavior behavior = Behavior::Constant;
    if (mEmitterPtr->flags().isSet(Ptcl::EmitterFlag::ColorRandom))
        behavior = Behavior::Random;
    else if (mEmitterPtr->flags().isSet(Ptcl::EmitterFlag::ColorAnimation))
        behavior = Behavior::Animation;

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
    for (int i = 0; i < mColorWidgets.size(); ++i)
        mColorWidgets[i].setVisible(visibility[i]);
}

void ColorPropertiesWidget::setColorLabels(const std::array<QString, 3>& labels) {
    for (int i = 0; i < mColorLabels.size(); ++i) {
        mColorLabels[i].setText(labels[i]);
        mColorLabels[i].setVisible(!labels[i].isEmpty());
    }
}

void ColorPropertiesWidget::handleBehaviorChanged(int index) {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    auto behavior = behaviorFromIndex(index);

    applyBehaviorToUI(behavior);

    switch (behavior) {
    case Behavior::Constant:
        mEmitterPtr->flags().clear(Ptcl::EmitterFlag::ColorRandom);
        mEmitterPtr->flags().clear(Ptcl::EmitterFlag::ColorAnimation);
        break;
    case Behavior::Random:
        mEmitterPtr->flags().set(Ptcl::EmitterFlag::ColorRandom);
        mEmitterPtr->flags().clear(Ptcl::EmitterFlag::ColorAnimation);
        break;
    case Behavior::Animation:
        mEmitterPtr->flags().clear(Ptcl::EmitterFlag::ColorRandom);
        mEmitterPtr->flags().set(Ptcl::EmitterFlag::ColorAnimation);
        break;
    }
}

void ColorPropertiesWidget::handleColorChanged() {
    if (mIsPopulating) {
        return;
    }

    auto* widget = qobject_cast<RGBAColorWidget*>(sender());
    if (!widget) {
        return;
    }

    int index = widget->property("colorIndex").toInt();
    if (index < 0 || index >= mColorWidgets.size()) {
        return;
    }

    const auto& color = widget->color();
    mEmitterPtr->setColor(index, color);

    QColor qcolor = QColor::fromRgbF(color.r, color.g, color.b);
    if (index == 0) mColorSections.setInitialColor(qcolor);
    else if (index == 1) mColorSections.setPeakColor(qcolor);
    else if (index == 2) mColorSections.setEndColor(qcolor);
}

void ColorPropertiesWidget::updateColorSection(ColorGradientEditor::HandleType handleType) {
    if (!mEmitterPtr) {
        return;
    }

    switch (handleType) {
    case ColorGradientEditor::HandleType::InCompletedHandle:
        mEmitterPtr->setColorSection1(mColorSections.inCompletedTiming());
        break;
    case ColorGradientEditor::HandleType::PeakHandle:
        mEmitterPtr->setColorSection2(mColorSections.peakTiming());
        break;
    case ColorGradientEditor::HandleType::OutStartHandle:
        mEmitterPtr->setColorSection3(mColorSections.outStartTiming());
        break;
    }
}
