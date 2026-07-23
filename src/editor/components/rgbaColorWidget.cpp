#include "editor/components/rgbaColorWidget.h"

#include "util/paintUtil.h"

#include <QColorDialog>
#include <QHBoxLayout>
#include <QPainter>
#include <QPainterPath>


// ==========================================================================//


ColorChannelRow::ColorChannelRow(const QString& name, Gfx::Color::Channel channel, QWidget* parent) :
    QWidget{parent}, mChannel{channel} {

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

    mSlider = new ColorChannelSlider(Qt::Horizontal, this);
    mSlider->setChannel(mChannel);
    mSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    layout->addWidget(&mLabel);
    layout->addWidget(mSlider);
    layout->addWidget(&mSpinBox);

    connect(&mSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ColorChannelRow::spinChanged);
    connect(mSlider, &QSlider::valueChanged, this, &ColorChannelRow::sliderChanged);
    connect(mSlider, &QSlider::sliderReleased, this, [this]() {
        emit valueCommitted(mChannel, mColor[mChannel]);
    });

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
}

void ColorChannelRow::setColor(const Gfx::Color& color) {
    mColor = color;

    QSignalBlocker b1(mSlider);
    QSignalBlocker b2(&mSpinBox);

    const f32 value = color[mChannel];
    mSpinBox.setValue(value);
    mSlider->setColor(mColor);
}

void ColorChannelRow::spinChanged(double value) {
    mColor[mChannel] = static_cast<f32>(value);

    QSignalBlocker b1(mSlider);
    mSlider->setColor(mColor);
    emit valueCommitted(mChannel, static_cast<f32>(value));
}

void ColorChannelRow::sliderChanged(s32 value) {
    f32 v = static_cast<f32>(value) / 255.0f;
    mColor[mChannel] = v;


    QSignalBlocker b1(&mSpinBox);
    mSpinBox.setValue(v);

    emit valueChanging(mChannel, v);
}


// ==========================================================================//


RGBAColorWidget::RGBAColorWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(2);

    // Top row: expand button + preview swatch
    auto* topRow = new QHBoxLayout();
    topRow->setContentsMargins(0, 0, 0, 0);
    topRow->setSpacing(2);

    mExpandBtn.setToolButtonStyle(Qt::ToolButtonIconOnly);
    mExpandBtn.setAutoRaise(true);
    mExpandBtn.setFixedSize(16, 16);
    mExpandBtn.setArrowType(Qt::RightArrow);
    mExpandBtn.setToolTip("Expand to show channel sliders");
    connect(&mExpandBtn, &QToolButton::clicked, this, &RGBAColorWidget::toggleExpanded);

    mPreview.setFixedWidth(32);
    mPreview.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    mPreview.setAutoFillBackground(true);
    connect(&mPreview, &ClickableLabel::clicked, this, &RGBAColorWidget::openColorDialog);

    topRow->addWidget(&mExpandBtn);
    topRow->addWidget(&mPreview);
    topRow->addStretch(1);

    mainLayout->addLayout(topRow);

    // Slider rows (hidden by default)
    mSliderLayout = new QVBoxLayout();
    mSliderLayout->setContentsMargins(0, 0, 0, 0);
    mSliderLayout->setSpacing(2);

    mRowR = new ColorChannelRow("R", Gfx::Color::Channel::R, this);
    mRowG = new ColorChannelRow("G", Gfx::Color::Channel::G, this);
    mRowB = new ColorChannelRow("B", Gfx::Color::Channel::B, this);
    mRowA = new ColorChannelRow("A", Gfx::Color::Channel::A, this);

    mRows = {mRowR, mRowG, mRowB, mRowA};
    for (auto* row : mRows) {
        row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        mSliderLayout->addWidget(row);
        connect(row, &ColorChannelRow::valueChanging, this, &RGBAColorWidget::onChannelChanged);
        connect(row, &ColorChannelRow::valueCommitted, this, [this](Gfx::Color::Channel channel, f32 value) {
            onChannelChanged(channel, value);
            emit colorChanged();
        });
    }

    mRowsContainer = new QWidget(this);
    mRowsContainer->setLayout(mSliderLayout);
    mRowsContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    mRowsContainer->hide();

    mainLayout->addWidget(mRowsContainer);

    setCollapsed(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
}

void RGBAColorWidget::setCollapsed(bool collapsed) {
    mCollapsed = collapsed;
    mRowsContainer->setVisible(!collapsed);

    if (collapsed) {
        mExpandBtn.setArrowType(Qt::RightArrow);
        mExpandBtn.setToolTip("Expand to show channel sliders");
    } else {
        mExpandBtn.setArrowType(Qt::DownArrow);
        mExpandBtn.setToolTip("Collapse channel sliders");
    }

    mPreview.setFixedHeight(collapsed ? 24 : mRowsContainer->sizeHint().height());
}

void RGBAColorWidget::toggleExpanded() {
    setCollapsed(!mCollapsed);
}

void RGBAColorWidget::enableAlpha(bool enabled) {
    mRowA->setVisible(enabled);
    mRowsContainer->updateGeometry();
    mPreview.setFixedHeight(mCollapsed ? 24 : mRowsContainer->sizeHint().height());
}

void RGBAColorWidget::setColor(const Gfx::Color& color) {
    mColor = color;

    for (auto* row : mRows) {
        row->setColor(color);
    }

    updatePreview();
    update();
}

void RGBAColorWidget::onChannelChanged(Gfx::Color::Channel channel, f32 value) {
    mColor[channel] = value;

    for (auto* row : mRows) {
        row->setColor(mColor);
    }

    updatePreview();
}

void RGBAColorWidget::openColorDialog() {
    QColorDialog colorDialog(mColor.toQColor(), this);
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
        emit colorChanged();
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

    PaintUtil::drawCheckerboard(painter, pixmap.rect(), 8, pixmap.size(), QColor(220, 220, 220), QColor(180, 180, 180));

    painter.setBrush(mColor.toQColor());
    painter.setPen(mColor.toQColor());
    painter.drawRoundedRect(roundedRect, radius, radius);

    mPreview.setPixmap(pixmap);
}


// ==========================================================================//
