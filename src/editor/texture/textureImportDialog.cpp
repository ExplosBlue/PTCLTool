#include "editor/texture/textureImportDialog.h"
#include "util/imageUtil.h"
#include "util/stringUtil.h"

#include <QtConcurrentRun>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>


// ========================================================================== //


static const std::array textureFormatOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::RGBA8888, "RGBA8888", "32-bit color with alpha channel." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::RGB888,   "RGB888",   "24-bit color, no alpha." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::RGBA5551, "RGBA5551", "16-bit color: 5 bits per channel, 1-bit alpha." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::RGB565,   "RGB565",   "16-bit color: 5-6-5 bit distribution, no alpha." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::RGBA4444, "RGBA4444", "16-bit color: 4 bits per channel including alpha." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::LA88,     "LA88",     "16-bit luminance + alpha, 8 bits each." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::HL8,      "HL8",      "8-bit high luminance." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::L8,       "L8",       "8-bit luminance." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::A8,       "A8",       "8-bit alpha only." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::LA44,     "LA44",     "8-bit luminance + alpha, 4 bits each." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::L4,       "L4",       "4-bit luminance." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::A4,       "A4",       "4-bit alpha only." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::ETC1,     "ETC1",     "Ericsson Texture Compression, no alpha." },
    EnumOption<Ptcl::TextureFormat>{ Ptcl::TextureFormat::ETC1_A4,  "ETC1_A4",  "ETC1 with a 4-bit alpha channel." },
};

static const std::array etcQualityOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<ImageUtil::ETC1Quality>{ ImageUtil::ETC1Quality::LowQuality,    "Low Quality",    "Fastest compression, lowest visual quality." },
    EnumOption<ImageUtil::ETC1Quality>{ ImageUtil::ETC1Quality::MediumQuality, "Medium Quality", "Balanced compression speed and quality." },
    EnumOption<ImageUtil::ETC1Quality>{ ImageUtil::ETC1Quality::HighQuality,   "High Quality",   "Best visual quality, slowest compression." },
};

static const std::array adjustModeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<PtclEditor::TextureAdjustMode>{ PtclEditor::TextureAdjustMode::None,    "None",    "No adjustment. Cropped image is centered on the canvas." },
    EnumOption<PtclEditor::TextureAdjustMode>{ PtclEditor::TextureAdjustMode::Stretch, "Stretch", "Cropped image is stretched to fill the entire canvas." },
    EnumOption<PtclEditor::TextureAdjustMode>{ PtclEditor::TextureAdjustMode::Fit,     "Fit",     "Cropped image is scaled to fit within the canvas, preserving aspect ratio." },
};

static const std::array filterOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Qt::TransformationMode>{ Qt::FastTransformation,   "Nearest Neighbor", "Sharp edges, no blending." },
    EnumOption<Qt::TransformationMode>{ Qt::SmoothTransformation, "Bilinear",         "Smooth interpolation." },
};


// ========================================================================== //


TextureImportDialog::TextureImportDialog(QWidget* parent, Qt::WindowFlags flags) :
    QDialog{parent, flags}, mTexture{nullptr} {
    setupFormatControls();
    setupAdjustmentControls();
    setupPreviewWidgets();
    setupConnections();

    auto* gridLayout = new QGridLayout;

    s32 row = 0;
    gridLayout->addWidget(new QLabel("Texture Format:"), row, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(&mFormatSelector, row++, 1);

    gridLayout->addWidget(new QLabel("Compression Quality:"), row, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(&mETCQuality, row++, 1);

    gridLayout->addWidget(&mETCDither, row++, 1);

    gridLayout->addWidget(&mAdjustGroup, row++, 0, 1, 2);

    auto* originalLabel = new QLabel("Original");
    auto* previewLabel  = new QLabel("Preview");
    originalLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setAlignment(Qt::AlignCenter);
    originalLabel->setStyleSheet("font-weight: bold;");
    previewLabel->setStyleSheet("font-weight: bold;");
    gridLayout->addWidget(originalLabel, row, 0);
    gridLayout->addWidget(previewLabel, row++, 1);

    gridLayout->addWidget(&mImportPreview, row, 0);
    gridLayout->addWidget(&mFormatPreview, row++, 1);

    auto* importZoomWidget = new QWidget;
    importZoomWidget->setLayout(buildImportZoomRow());
    auto* formatZoomWidget = new QWidget;
    formatZoomWidget->setLayout(buildFormatZoomRow());
    gridLayout->addWidget(importZoomWidget, row, 0);
    gridLayout->addWidget(formatZoomWidget, row++, 1);

    gridLayout->addWidget(&mInfoLabel, row++, 1);

    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, [this]() {
        mCanceled = true;
        mWatcher.cancel();
        mWatcher.waitForFinished();
        disconnect(&mWatcher, nullptr, this, nullptr);
        reject();
    });
    gridLayout->addWidget(buttonBox, row++, 0, 1, 2);

    gridLayout->setHorizontalSpacing(15);
    gridLayout->setVerticalSpacing(10);
    gridLayout->setContentsMargins(10, 10, 10, 10);

    setLayout(gridLayout);

    mOkButton = buttonBox->button(QDialogButtonBox::Ok);

    setFixedSize(sizeHint());
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinMaxButtonsHint);
}

TextureImportDialog::~TextureImportDialog() {
    mCanceled = true;
    mWatcher.cancel();
    mWatcher.waitForFinished();
}

void TextureImportDialog::setupFormatControls() {
    mFormatSelector.setOptions(textureFormatOptions);
    mFormatSelector.setToolTip("Target texture format.");

    mETCQuality.setOptions(etcQualityOptions);
    mETCQuality.setEnabled(false);
    mETCQuality.setCurrentEnum(ImageUtil::ETC1Quality::HighQuality);
    mETCQuality.setToolTip("Compression quality for ETC1 and ETC1_A4 formats.");

    mETCDither.setEnabled(false);
    mETCDither.setText("Enable Dithering");
    mETCDither.setToolTip("Apply dithering to reduce color banding in ETC-compressed textures.");
}

void TextureImportDialog::setupAdjustmentControls() {
    mAdjustMode.setOptions(adjustModeOptions);
    mAdjustMode.setDescription("How the cropped image is sized to fit the output canvas.");

    populateSizeCombo(mTargetWidth, 256);
    populateSizeCombo(mTargetHeight, 256);
    mTargetWidth.setToolTip("Output canvas width.");
    mTargetHeight.setToolTip("Output canvas height.");

    mFilter.setOptions(filterOptions);
    mFilter.setDescription("Interpolation used when stretching or fitting the image to the canvas.");

    auto* adjustLayout = new QHBoxLayout;
    adjustLayout->addWidget(new QLabel("Sizing:"));
    adjustLayout->addWidget(&mAdjustMode);
    adjustLayout->addSpacing(10);
    adjustLayout->addWidget(new QLabel("Canvas:"));
    adjustLayout->addWidget(&mTargetWidth);
    adjustLayout->addWidget(new QLabel("\u00D7"));
    adjustLayout->addWidget(&mTargetHeight);
    adjustLayout->addSpacing(10);
    adjustLayout->addWidget(new QLabel("Interpolation:"));
    adjustLayout->addWidget(&mFilter);
    adjustLayout->addStretch();

    mAdjustGroup.setTitle("Image Adjustment");
    mAdjustGroup.setLayout(adjustLayout);
}

static void setupZoomBtn(QPushButton& btn, const QString& text, const QString& tooltip) {
    btn.setText(text);
    btn.setToolTip(tooltip);
    btn.setFixedWidth(28);
    btn.setFixedHeight(24);
}

QHBoxLayout* TextureImportDialog::buildImportZoomRow() {
    setupZoomBtn(mImportZoomIn,  "+",  "Zoom in");
    setupZoomBtn(mImportZoomOut, "\u2212", "Zoom out");
    setupZoomBtn(mImportZoomReset, "\u21BA", "Reset zoom");

    mImportResetCrop.setText("Reset Crop");
    mImportResetCrop.setToolTip("Reset the crop region to the full source image.");
    mImportResetCrop.setFixedHeight(24);

    auto* row = new QHBoxLayout;
    row->addStretch();
    row->addWidget(&mImportZoomOut);
    row->addWidget(&mImportZoomReset);
    row->addWidget(&mImportZoomIn);
    row->addSpacing(12);
    row->addWidget(&mImportResetCrop);
    row->addStretch();
    return row;
}

QHBoxLayout* TextureImportDialog::buildFormatZoomRow() {
    setupZoomBtn(mFormatZoomIn,  "+",  "Zoom in");
    setupZoomBtn(mFormatZoomOut, "\u2212", "Zoom out");
    setupZoomBtn(mFormatZoomReset, "\u21BA", "Reset zoom");

    auto* row = new QHBoxLayout;
    row->addStretch();
    row->addWidget(&mFormatZoomOut);
    row->addWidget(&mFormatZoomReset);
    row->addWidget(&mFormatZoomIn);
    row->addStretch();
    return row;
}

void TextureImportDialog::setupPreviewWidgets() {
    mImportPreview.setThumbnailSize(QSize(256, 256));
    mImportPreview.setZoomEnabled(true);
    mImportPreview.setToolTip(
        "<b>Left-click</b> inside the crop region to reposition it.<br>"
        "<b>Right-click</b> and drag to resize the crop region.<br>"
        "<b>Hold Shift</b> while resizing to disable power-of-2 snapping.<br>"
        "<b>Scroll</b> to zoom.");

    mFormatPreview.setThumbnailSize(QSize(256, 256));
    mFormatPreview.setZoomEnabled(true);
    mFormatPreview.setToolTip("Scroll to zoom.");
}

void TextureImportDialog::setupConnections() {
    connect(&mFormatSelector, &QComboBox::currentIndexChanged, this, &TextureImportDialog::updateTextureFormat);
    connect(&mETCQuality, &QComboBox::currentIndexChanged, this, &TextureImportDialog::updateTextureFormat);
    connect(&mETCDither, &QCheckBox::checkStateChanged, this, &TextureImportDialog::updateTextureFormat);

    connect(&mAdjustMode, &QComboBox::currentIndexChanged, this, &TextureImportDialog::updateAdjustment);
    connect(&mTargetWidth, &QComboBox::currentIndexChanged, this, &TextureImportDialog::updateAdjustment);
    connect(&mTargetHeight, &QComboBox::currentIndexChanged, this, &TextureImportDialog::updateAdjustment);
    connect(&mFilter, &QComboBox::currentIndexChanged, this, &TextureImportDialog::updateAdjustment);

    connect(&mImportZoomIn, &QPushButton::clicked, &mImportPreview, &ThumbnailWidget::zoomIn);
    connect(&mImportZoomOut, &QPushButton::clicked, &mImportPreview, &ThumbnailWidget::zoomOut);
    connect(&mImportZoomReset, &QPushButton::clicked, &mImportPreview, &ThumbnailWidget::resetView);
    connect(&mImportResetCrop, &QPushButton::clicked, this, [this]() {
        mImportPreview.setCropRect(mImage.rect());
        updateAdjustment();
    });
    connect(&mFormatZoomIn, &QPushButton::clicked, &mFormatPreview, &ThumbnailWidget::zoomIn);
    connect(&mFormatZoomOut, &QPushButton::clicked, &mFormatPreview, &ThumbnailWidget::zoomOut);
    connect(&mFormatZoomReset, &QPushButton::clicked, &mFormatPreview, &ThumbnailWidget::resetView);

    connect(&mWatcher, &QFutureWatcher<std::unique_ptr<Ptcl::Texture>>::finished, this, [this]() {
        mLoadingSpinner.stop();
        auto result = mWatcher.future().takeResult();
        if (result) {
            mTexture = std::move(result);
            mFormatPreview.setPixmap(QPixmap::fromImage(mTexture->textureData()));
            updateTextureInfo();
        }
        mOkButton->setEnabled(true);
    });

    connect(&mImportPreview, &CropWidget::cropChanged, this, [this]([[maybe_unused]] QRect rect) {
        updateAdjustment();
    });

    connect(&mLoadingSpinner, &LoadingSpinner::frameUpdated, this, [this](const QPixmap& pixmap) {
        mFormatPreview.setPixmap(pixmap);
    });

    mInfoLabel.setText("Dimensions: N/A\nSize: N/A");
    mInfoLabel.setAlignment(Qt::AlignRight);

}

void TextureImportDialog::setFilePath(const QString& path) {
    mFilePath = path;
    updateImportPreview();
}

std::unique_ptr<Ptcl::Texture> TextureImportDialog::getTexture() {
    return std::move(mTexture);
}

PtclEditor::TextureImportSettings TextureImportDialog::buildSettings() const {
    PtclEditor::TextureImportSettings s;
    s.mode = mAdjustMode.currentEnum();
    s.canvasSize = QSize(mTargetWidth.currentText().toInt(), mTargetHeight.currentText().toInt());
    s.cropRect = mImportPreview.cropRect();
    s.filter = mFilter.currentEnum();
    s.format = mFormatSelector.currentEnum();
    s.quality = mETCQuality.currentEnum();
    s.dither = mETCDither.isChecked();
    return s;
}

void TextureImportDialog::updateTextureFormat() {
    auto currentFormat = mFormatSelector.currentEnum();

    if (currentFormat == Ptcl::TextureFormat::ETC1 || currentFormat == Ptcl::TextureFormat::ETC1_A4) {
        mETCQuality.setEnabled(true);
        mETCDither.setEnabled(true);
    } else {
        mETCQuality.setEnabled(false);
        mETCDither.setEnabled(false);
    }

    mProcessor.setSettings(buildSettings());
    updateFormatPreview();
}

void TextureImportDialog::updateImportPreview() {
    mImage.load(mFilePath);

    if (mImage.isNull()) {
        mImportPreview.clear();
        mFormatPreview.clear();
        mOkButton->setEnabled(false);
        return;
    }

    mProcessor.setSourceImage(mImage);

    mImportPreview.setImage(mImage);
    mImportPreview.setCropEnabled(true);
    mOkButton->setEnabled(true);

    auto nearestW = ImageUtil::nearestValidTextureSize(mImage.width());
    auto nearestH = ImageUtil::nearestValidTextureSize(mImage.height());
    populateSizeCombo(mTargetWidth, nearestW);
    populateSizeCombo(mTargetHeight, nearestH);

    mAdjustMode.setCurrentIndex(0);

    updateAdjustment();
}

void TextureImportDialog::updateFormatPreview() {
    const QImage& adjusted = mProcessor.adjustedImage();
    if (adjusted.isNull()) {
        mLoadingSpinner.stop();
        mFormatPreview.clear();
        return;
    }

    if (!ImageUtil::isValidTextureSize(adjusted.width(), adjusted.height())) {
        mLoadingSpinner.stop();
        mFormatPreview.clear();
        return;
    }

    mLoadingSpinner.start();

    mOkButton->setEnabled(false);
    mCanceled = false;
    mWatcher.cancel();

    auto processorCopy = mProcessor;

    QFuture<std::unique_ptr<Ptcl::Texture>> future = QtConcurrent::run(
        [processorCopy, &cancelFlag = mCanceled]() {
            if (cancelFlag.load()) { return std::unique_ptr<Ptcl::Texture>(nullptr); }
            return processorCopy.buildTexture(cancelFlag);
        });

    mWatcher.setFuture(future);
}

void TextureImportDialog::updateOkButton() {
    mOkButton->setEnabled(ImageUtil::isValidTextureSize(mProcessor.adjustedImage().width(), mProcessor.adjustedImage().height()));
}

void TextureImportDialog::updateAdjustment() {
    if (mImage.isNull()) {
        return;
    }

    QRect cr = mImportPreview.cropRect();
    if (!cr.isValid()) {
        cr = mImage.rect();
        mImportPreview.setCropRect(cr);
    }

    mProcessor.setSettings(buildSettings());
    updateOkButton();
    updateFormatPreview();
}

void TextureImportDialog::updateTextureInfo() {
    auto width = mTexture->textureData().width();
    auto height = mTexture->textureData().height();

    const QString sizeString = StringUtil::formatBytes(mTexture->textureDataRaw().size());

    auto text = QString("Dimensions: %1\u00D7%2\nSize: %3")
        .arg(width)
        .arg(height)
        .arg(sizeString);

    mInfoLabel.setText(text);
}

void TextureImportDialog::populateSizeCombo(QComboBox& combo, s32 initial) {
    combo.blockSignals(true);
    combo.clear();
    for (s32 size : {8, 16, 32, 64, 128, 256, 512}) {
        combo.addItem(QString::number(size), size);
    }
    s32 nearest = ImageUtil::nearestValidTextureSize(initial);
    s32 idx = combo.findText(QString::number(nearest));
    if (idx >= 0) {
        combo.setCurrentIndex(idx);
    }
    combo.blockSignals(false);
}


// ========================================================================== //
