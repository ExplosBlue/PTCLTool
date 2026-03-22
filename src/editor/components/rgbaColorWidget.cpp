#include "editor/components/rgbaColorWidget.h"

#include "util/paintUtil.h"

#include <QColorDialog>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>


// ========================================================================== //


ColorChannelRow::ColorChannelRow(const QString& name, QSlider* slider, QWidget* parent) :
    QWidget{parent}, mSlider{slider} {

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    mLabel.setText(name);
    mLabel.setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    mLabel.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    mSpinBox.setDecimals(3);
    mSpinBox.setRange(0.0, 1.0);
    mSpinBox.setSingleStep(0.01);
    mSpinBox.setButtonSymbols(QAbstractSpinBox::NoButtons);
    mSpinBox.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    mSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(&mLabel);
    layout->addWidget(mSlider);
    layout->addWidget(&mSpinBox);

    connect(&mSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ColorChannelRow::spinChanged);
    connect(mSlider, &QSlider::valueChanged, this, &ColorChannelRow::sliderChanged);
}


// ========================================================================== //


RGBAColorWidget::RGBAColorWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(4);

    mSliderLayout = new QVBoxLayout();
    mSliderLayout->setContentsMargins(0, 0, 0 ,0);
    mSliderLayout->setSpacing(2);

    mSliderR = new GradientSlider(Qt::Horizontal, this);
    mSliderR->setChannel(GradientSlider::ChannelType::Red);

    mSliderG = new GradientSlider(Qt::Horizontal, this);
    mSliderG->setChannel(GradientSlider::ChannelType::Green);

    mSliderB = new GradientSlider(Qt::Horizontal, this);
    mSliderB->setChannel(GradientSlider::ChannelType::Blue);

    mSliderA = new AlphaSlider(Qt::Horizontal, this);

    mRowR = new ColorChannelRow("R", mSliderR, this);
    mRowG = new ColorChannelRow("G", mSliderG, this);
    mRowB = new ColorChannelRow("B", mSliderB, this);
    mRowA = new ColorChannelRow("A", mSliderA, this);

    mRows = {mRowR, mRowG, mRowB, mRowA};
    for (auto* row : mRows) {
        row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        mSliderLayout->addWidget(row);
        connect(row, &ColorChannelRow::spinChanged, this, &RGBAColorWidget::updateColorFromSpinBoxes);
        connect(row, &ColorChannelRow::sliderChanged, this, &RGBAColorWidget::updateColorFromSliders);
    }

    mRowsContainer = new QWidget(this);
    mRowsContainer->setLayout(mSliderLayout);
    mRowsContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    mPreview.setFixedWidth(32);
    mPreview.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    mPreview.setFixedHeight(mRowsContainer->sizeHint().height());
    mPreview.setAutoFillBackground(true);
    connect(&mPreview, &ClickableLabel::clicked, this, &RGBAColorWidget::openColorDialog);

    mainLayout->addWidget(mRowsContainer);
    mainLayout->addWidget(&mPreview, 0, Qt::AlignTop);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

void RGBAColorWidget::enableAlpha(bool enabled) {
    mRowA->setVisible(enabled);
    mRowsContainer->updateGeometry();
    mPreview.setFixedHeight(mRowsContainer->sizeHint().height());
}

void RGBAColorWidget::setColor(const Gfx::Color& color) {
    QSignalBlocker b1(mSliderR);
    QSignalBlocker b2(mSliderG);
    QSignalBlocker b3(mSliderB);
    QSignalBlocker b4(mSliderA);

    QSignalBlocker b5(mRowR->spinBox());
    QSignalBlocker b6(mRowB->spinBox());
    QSignalBlocker b7(mRowG->spinBox());
    QSignalBlocker b8(mRowA->spinBox());

    s32 r = static_cast<s32>(std::round(color.r() * 255.0f));
    s32 g = static_cast<s32>(std::round(color.g() * 255.0f));
    s32 b = static_cast<s32>(std::round(color.b() * 255.0f));
    s32 a = static_cast<s32>(std::round(color.a() * 255.0f));

    mSliderR->setRgbValues(r, g, b);
    mSliderG->setRgbValues(r, g, b);
    mSliderB->setRgbValues(r, g, b);
    mSliderA->setColor(r, g, b);

    mSliderR->setValue(r);
    mSliderG->setValue(g);
    mSliderB->setValue(b);
    mSliderA->setValue(a);

    mRowR->spinBox()->setValue(color.r());
    mRowG->spinBox()->setValue(color.g());
    mRowB->spinBox()->setValue(color.b());
    mRowA->spinBox()->setValue(color.a());

    updatePreview();
    update();
}

Gfx::Color RGBAColorWidget::color() const {
    return {
        static_cast<f32>(mRowR->spinBox()->value()),
        static_cast<f32>(mRowG->spinBox()->value()),
        static_cast<f32>(mRowB->spinBox()->value()),
        static_cast<f32>(mRowA->spinBox()->value()),
    };
}

QColor RGBAColorWidget::toQColor() const {
    return {
        mSliderR->value(),
        mSliderG->value(),
        mSliderB->value(),
        mSliderA->value()
    };
}

void RGBAColorWidget::updateColorFromSliders() {
    blockSignals(true);
    QSignalBlocker b1(mRowR->spinBox());
    QSignalBlocker b2(mRowG->spinBox());
    QSignalBlocker b3(mRowB->spinBox());
    QSignalBlocker b4(mRowA->spinBox());

    s32 r = mSliderR->value();
    s32 g = mSliderG->value();
    s32 b = mSliderB->value();
    s32 a = mSliderA->value();

    mSliderR->setRgbValues(0, g, b);
    mSliderG->setRgbValues(r, 0, b);
    mSliderB->setRgbValues(r, g, 0);
    mSliderA->setColor(r, g, b);

    double rFloat = static_cast<double>(r) / 255.0f;
    double gFloat = static_cast<double>(g) / 255.0f;
    double bFloat = static_cast<double>(b) / 255.0f;
    double aFloat = static_cast<double>(a) / 255.0f;

    mRowR->spinBox()->setValue(rFloat);
    mRowG->spinBox()->setValue(gFloat);
    mRowB->spinBox()->setValue(bFloat);
    mRowA->spinBox()->setValue(aFloat);

    blockSignals(false);

    updatePreview();
    emit colorChanged();
}

void RGBAColorWidget::updateColorFromSpinBoxes() {
    blockSignals(true);
    QSignalBlocker b1(mSliderR);
    QSignalBlocker b2(mSliderG);
    QSignalBlocker b3(mSliderB);
    QSignalBlocker b4(mSliderA);

    double rFloat = mRowR->spinBox()->value();
    double gFloat = mRowG->spinBox()->value();
    double bFloat = mRowB->spinBox()->value();
    double aFloat = mRowA->spinBox()->value();

    s32 r = static_cast<s32>(std::round(rFloat * 255.0f));
    s32 g = static_cast<s32>(std::round(gFloat * 255.0f));
    s32 b = static_cast<s32>(std::round(bFloat * 255.0f));
    s32 a = static_cast<s32>(std::round(aFloat * 255.0f));

    mSliderR->setRgbValues(0, g, b);
    mSliderG->setRgbValues(r, 0, b);
    mSliderB->setRgbValues(r, g, 0);
    mSliderA->setColor(r, g, b);

    mSliderR->setValue(r);
    mSliderG->setValue(g);
    mSliderB->setValue(b);
    mSliderA->setValue(a);

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

        Gfx::Color newColorFloat {
            newColor.redF(),
            newColor.greenF(),
            newColor.blueF(),
            newColor.alphaF()
        };
        setColor(newColorFloat);
    }
}

void RGBAColorWidget::updatePreview() {
    QPixmap pixmap(mPreview.size());
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    const s32 radius = 5;
    QRectF roundedRect = pixmap.rect();
    roundedRect = QRectF(
        std::ceil(roundedRect.left()) + 0.5,
        std::ceil(roundedRect.top()) + 0.5,
        std::floor(roundedRect.width()) - 1,
        std::floor(roundedRect.height()) - 1);

    QPainterPath path;
    path.addRoundedRect(roundedRect, radius, radius);
    painter.setClipPath(path);

    PaintUtil::drawCheckerboard(painter, pixmap.rect(), 6, QColor(220, 220, 220), QColor(180, 180, 180));

    painter.setBrush(toQColor());
    painter.setPen(toQColor());
    painter.drawRoundedRect(roundedRect, radius, radius);

    mPreview.setPixmap(pixmap);
}


// ========================================================================== //
