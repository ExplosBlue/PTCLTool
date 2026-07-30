#include "editor/texture/textureImportProcessor.h"


// ========================================================================== //


namespace PtclEditor {


QImage centerOnCanvas(const QImage& image, QSize canvasSize) {
    if (image.size() == canvasSize) {
        return image;
    }

    QImage canvas(canvasSize, QImage::Format_ARGB32_Premultiplied);
    canvas.fill(Qt::transparent);

    QPainter painter(&canvas);
    painter.drawImage(
        (canvas.width() - image.width()) / 2,
        (canvas.height() - image.height()) / 2,
        image
    );

    return canvas;
}


// ========================================================================== //


void TextureImportProcessor::setSourceImage(QImage image) {
    mSourceImage = std::move(image);
}

void TextureImportProcessor::setSettings(const TextureImportSettings& settings) {
    mSettings = settings;

    QImage source = mSettings.cropRect.isValid()
        ? mSourceImage.copy(mSettings.cropRect)
        : mSourceImage;

    QSize canvas = mSettings.canvasSize;

    QImage result = source;

    switch (mSettings.mode) {
    case TextureAdjustMode::None:
        break;
    case TextureAdjustMode::Stretch:
        result = source.scaled(canvas, Qt::IgnoreAspectRatio, mSettings.filter);
        break;
    case TextureAdjustMode::Fit:
        result = source.scaled(canvas, Qt::KeepAspectRatio, mSettings.filter);
        break;
    }

    mAdjustedImage = centerOnCanvas(result, canvas);
}

const QImage& TextureImportProcessor::adjustedImage() const {
    return mAdjustedImage;
}

std::unique_ptr<Ptcl::Texture> TextureImportProcessor::buildTexture(const std::atomic<bool>& cancelFlag) const {
    auto data = ImageUtil::QImageToPicaTexture(mAdjustedImage, mSettings.format, mSettings.quality, mSettings.dither, cancelFlag);
    if (data.empty()) {
        return nullptr;
    }
    return std::make_unique<Ptcl::Texture>(&data, mAdjustedImage.width(), mAdjustedImage.height(), mSettings.format);
}

} // namespace PtclEditor
