#include "editor/rgbaColorWidget.h"

#include <QColorDialog>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>

#include <variant>


// ========================================================================== //


RGBAColorWidget::RGBAColorWidget(QWidget* parent) :
    QWidget(parent), mSliderR{Qt::Horizontal, this}, mSliderG{Qt::Horizontal, this},
    mSliderB{Qt::Horizontal, this}, mSliderA{Qt::Horizontal, this} {

    auto* mainLayout = new QHBoxLayout(this);
    auto* sliderLayout = new QVBoxLayout();

    using SliderType = std::variant<GradientSlider*, AlphaSlider*>;

    auto addChannelRow = [&](const QString& name, SliderType sliderType, QDoubleSpinBox* spinbox) {
        auto* row = new QHBoxLayout;
        row->addWidget(new QLabel(name));
        row->addWidget(spinbox);
        spinbox->setDecimals(3);
        spinbox->setRange(0.0, 1.0);
        spinbox->setSingleStep(0.01);
        spinbox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        connect(spinbox, &QDoubleSpinBox::valueChanged, this, &RGBAColorWidget::updateColorFromSpinBoxes);

        std::visit([&](auto* slider) {
            row->addWidget(slider);
            connect(slider, &QSlider::valueChanged, this, &RGBAColorWidget::updateColorFromSliders);
        }, sliderType);

        sliderLayout->addLayout(row);
    };

    addChannelRow("R", &mSliderR, &mSpinBoxR);
    mSliderR.setChannel(GradientSlider::ChannelType::Red);
    addChannelRow("G", &mSliderG, &mSpinBoxG);
    mSliderG.setChannel(GradientSlider::ChannelType::Green);
    addChannelRow("B", &mSliderB, &mSpinBoxB);
    mSliderB.setChannel(GradientSlider::ChannelType::Blue);
    addChannelRow("A", &mSliderA, &mSpinBoxA);

    mPreview.setFixedWidth(50);
    mPreview.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mPreview.setAutoFillBackground(true);
    connect(&mPreview, &ClickableLabel::clicked, this, &RGBAColorWidget::openColorDialog);

    mainLayout->addWidget(&mPreview, 0, Qt::AlignTop);
    mainLayout->addLayout(sliderLayout);

    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

    mPreview.setFixedHeight(sliderLayout->sizeHint().height());
    updatePreview();
}

void RGBAColorWidget::setColor(const Ptcl::binColor4f& color) {
    int r = static_cast<int>(std::round(color.r * 255.0f));
    int g = static_cast<int>(std::round(color.g * 255.0f));
    int b = static_cast<int>(std::round(color.b * 255.0f));
    int a = static_cast<int>(std::round(color.a * 255.0f));

    mSliderR.setRgbValues(r, g, b);
    mSliderG.setRgbValues(r, g, b);
    mSliderB.setRgbValues(r, g, b);
    mSliderA.setColor(r, g, b);

    mSliderR.setValue(r);
    mSliderG.setValue(g);
    mSliderB.setValue(b);
    mSliderA.setValue(a);

    mSpinBoxR.setValue(color.r);
    mSpinBoxG.setValue(color.g);
    mSpinBoxB.setValue(color.b);
    mSpinBoxA.setValue(color.a);

    updatePreview();
    update();
}

Ptcl::binColor4f RGBAColorWidget::color() const {
    Ptcl::binColor4f result;

    result.r = static_cast<float>(mSpinBoxR.value());
    result.g = static_cast<float>(mSpinBoxG.value());
    result.b = static_cast<float>(mSpinBoxB.value());
    result.a = static_cast<float>(mSpinBoxA.value());
    return result;
}

QColor RGBAColorWidget::toQColor() const {
    return {mSliderR.value(), mSliderG.value(), mSliderB.value(), mSliderA.value()};
}

void RGBAColorWidget::updateColorFromSliders() {
    blockSignals(true);
    mSpinBoxR.blockSignals(true);
    mSpinBoxG.blockSignals(true);
    mSpinBoxB.blockSignals(true);
    mSpinBoxA.blockSignals(true);

    int r = mSliderR.value();
    int g = mSliderG.value();
    int b = mSliderB.value();
    int a = mSliderA.value();

    mSliderR.setRgbValues(0, g, b);
    mSliderG.setRgbValues(r, 0, b);
    mSliderB.setRgbValues(r, g, 0);
    mSliderA.setColor(r, g, b);

    double rFloat = static_cast<double>(r) / 255.0f;
    double gFloat = static_cast<double>(g) / 255.0f;
    double bFloat = static_cast<double>(b) / 255.0f;
    double aFloat = static_cast<double>(a) / 255.0f;

    mSpinBoxR.setValue(rFloat);
    mSpinBoxG.setValue(gFloat);
    mSpinBoxB.setValue(bFloat);
    mSpinBoxA.setValue(aFloat);

    mSpinBoxR.blockSignals(false);
    mSpinBoxG.blockSignals(false);
    mSpinBoxB.blockSignals(false);
    mSpinBoxA.blockSignals(false);
    blockSignals(false);

    updatePreview();
    emit colorChanged();
}

void RGBAColorWidget::updateColorFromSpinBoxes() {
    blockSignals(true);
    mSliderR.blockSignals(true);
    mSliderG.blockSignals(true);
    mSliderB.blockSignals(true);
    mSliderA.blockSignals(true);

    double rFloat = mSpinBoxR.value();
    double gFloat = mSpinBoxG.value();
    double bFloat = mSpinBoxB.value();
    double aFloat = mSpinBoxA.value();

    int r = static_cast<int>(std::round(rFloat * 255.0f));
    int g = static_cast<int>(std::round(gFloat * 255.0f));
    int b = static_cast<int>(std::round(bFloat * 255.0f));
    int a = static_cast<int>(std::round(aFloat * 255.0f));

    mSliderR.setRgbValues(0, g, b);
    mSliderG.setRgbValues(r, 0, b);
    mSliderB.setRgbValues(r, g, 0);
    mSliderA.setColor(r, g, b);

    mSliderR.setValue(r);
    mSliderG.setValue(g);
    mSliderB.setValue(b);
    mSliderA.setValue(a);

    mSliderR.blockSignals(false);
    mSliderG.blockSignals(false);
    mSliderA.blockSignals(false);
    mSliderB.blockSignals(false);
    blockSignals(false);

    updatePreview();
    emit colorChanged();
}

void RGBAColorWidget::openColorDialog() {
    QColorDialog colorDialog(toQColor(), this);
    colorDialog.setOption(QColorDialog::ShowAlphaChannel);
    colorDialog.setWindowTitle("Select Color");

    if (colorDialog.exec() == QDialog::Accepted) {
        QColor newColor = colorDialog.selectedColor();

        Ptcl::binColor4f newColorFloat;
        newColorFloat.r = newColor.redF();
        newColorFloat.g = newColor.greenF();
        newColorFloat.b = newColor.blueF();
        newColorFloat.a = newColor.alphaF();
        setColor(newColorFloat);
    }
}

void RGBAColorWidget::updatePreview() {
    QPixmap pixmap(mPreview.size());
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    const int radius = 5;
    QRectF roundedRect = pixmap.rect();
    roundedRect = QRectF(
        std::ceil(roundedRect.left()) + 0.5,
        std::ceil(roundedRect.top()) + 0.5,
        std::floor(roundedRect.width()) - 1,
        std::floor(roundedRect.height()) - 1);

    QPainterPath path;
    path.addRoundedRect(roundedRect, radius, radius);
    painter.setClipPath(path);

    const int checkerSize = 6;
    QColor light(220, 220, 220);
    QColor dark(180, 180, 180);

    for (int y = 0; y < pixmap.height(); y += checkerSize) {
        for (int x = 0; x < pixmap.width(); x += checkerSize) {
            QRect cell(x, y, checkerSize, checkerSize);
            bool isLight = ((x / checkerSize + y / checkerSize) % 2) == 0;
            painter.fillRect(cell, isLight ? light : dark);
        }
    }

    painter.setBrush(toQColor());
    painter.setPen(toQColor());
    painter.drawRoundedRect(roundedRect, radius, radius);

    mPreview.setPixmap(pixmap);
}


// ========================================================================== //
