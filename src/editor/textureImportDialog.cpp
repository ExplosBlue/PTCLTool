#include "editor/textureImportDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>

#include "util/imageUtil.h"

TextureImportDialog::TextureImportDialog(QWidget* parent, Qt::WindowFlags flags) :
    QDialog{parent, flags}, mTexture{nullptr} {

    // Format Selector
    connect(&mFormatSelector, &QComboBox::currentIndexChanged, this, &TextureImportDialog::updateTextureFormat);

    // Import Preview
    mImportPreview.setThumbnailSize(QSize(256, 256));

    // Format Preview
    mFormatPreview.setThumbnailSize(QSize(256, 256));

    // Button Box
    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // ETC Quality
    mETCQuality.setEnabled(false);
    mETCQuality.setCurrentEnum(ImageUtil::ETC1Quality::HighQuality);
    connect(&mETCQuality, &QComboBox::currentIndexChanged, this, &TextureImportDialog::updateTextureFormat);

    // ETC1 Dither
    mETCDither.setEnabled(false);
    mETCDither.setText("Enable Dithering");
    connect(&mETCDither, &QCheckBox::checkStateChanged, this, &TextureImportDialog::updateTextureFormat);

    // Texture Info
    mInfoLabel.setText("Dimentions: N/A\nSize: N/A");
    mInfoLabel.setAlignment(Qt::AlignRight);

    QGridLayout* gridLayout = new QGridLayout;

    // Row 0: Format selector label + combo
    gridLayout->addWidget(new QLabel("Texture Format:"), 0, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(&mFormatSelector, 0, 1);

    // Row 1: ETC quality
    gridLayout->addWidget(new QLabel("Compression Quality:"), 1, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(&mETCQuality, 1, 1);

    // Row 2: Dithering controls
    gridLayout->addWidget(&mETCDither, 2, 1);

    // Row 3: Labels above previews
    QLabel* originalLabel = new QLabel("Original");
    QLabel* previewLabel = new QLabel("Preview");
    originalLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setAlignment(Qt::AlignCenter);
    originalLabel->setStyleSheet("font-weight: bold;");
    previewLabel->setStyleSheet("font-weight: bold;");

    gridLayout->addWidget(originalLabel, 3, 0);
    gridLayout->addWidget(previewLabel, 3, 1);

    // Row 4: The preview widgets
    gridLayout->addWidget(&mImportPreview, 4, 0);
    gridLayout->addWidget(&mFormatPreview, 4, 1);

    // Row 5: Texture Info
    gridLayout->addWidget(&mInfoLabel, 5, 1);

    // Row 6: Button box
    gridLayout->addWidget(buttonBox, 6, 0, 1, 2);

    // Optional: Add spacing and margins if needed
    gridLayout->setHorizontalSpacing(15);
    gridLayout->setVerticalSpacing(10);
    gridLayout->setContentsMargins(10, 10, 10, 10);

    setLayout(gridLayout);

    setFixedSize(sizeHint());
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint & ~Qt::WindowMinMaxButtonsHint);

    // Watcher
    connect(&mWatcher, &QFutureWatcher<std::vector<u8>>::finished, this, [this]() {
        mLoadingSpinner.stop();

        auto data = mWatcher.result();
        auto currentFormat = mFormatSelector.currentEnum();

        mTexture = std::make_unique<Ptcl::Texture>(&data, mImage.width(), mImage.height(), currentFormat);
        mFormatPreview.setPixmap(QPixmap::fromImage(mTexture->textureData()));
        updateTextureInfo();
    });

    // Loading spinner
    connect(&mLoadingSpinner, &LoadingSpinner::frameUpdated, this, [=, this](const QPixmap& pixmap) {
        mFormatPreview.setPixmap(pixmap);
    });

}

void TextureImportDialog::setFilePath(const QString& path) {
    mFilePath = path;
    updateImportPreview();
    updateFormatPreview();
}

std::unique_ptr<Ptcl::Texture> TextureImportDialog::getTexture() {
    return std::move(mTexture);
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

    updateFormatPreview();
}

void TextureImportDialog::updateImportPreview() {
    mImage.load(mFilePath);
    mImportPreview.setPixmap(QPixmap::fromImage(mImage));
}

void TextureImportDialog::updateFormatPreview() {
    mLoadingSpinner.start();

    auto currentFormat = mFormatSelector.currentEnum();
    auto currentQuality = mETCQuality.currentEnum();
    auto currentDither = mETCDither.isChecked();

    // Run QImageToPicaTexture asynchronously
    QFuture<std::vector<u8>> future = QtConcurrent::run([=, this]() {
        return ImageUtil::QImageToPicaTexture(mImage, currentFormat, currentQuality, currentDither);
    });

    mWatcher.setFuture(future);
}

void TextureImportDialog::updateTextureInfo() {
    auto width = mTexture->textureData().width();
    auto height = mTexture->textureData().height();
    auto sizeBytes = mTexture->textureDataRaw().size();

    QString sizeString;
    if (sizeBytes < 1024) {
        sizeString = QString("%1 Bytes").arg(sizeBytes);
    } else {
        sizeString = QString("%1 KB").arg(sizeBytes / 1024);
    }

    auto text = QString("Dimentions: %1x%2\nSize: %3")
        .arg(width)
        .arg(height)
        .arg(sizeString);

    mInfoLabel.setText(text);
}
