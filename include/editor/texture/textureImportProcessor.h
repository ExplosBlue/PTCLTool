#pragma once

#include "ptcl/ptclEnum.h"
#include "ptcl/ptclTexture.h"
#include "util/imageUtil.h"

#include <QImage>
#include <QPainter>
#include <QPoint>
#include <QSize>

#include <atomic>

#include <memory>


// ========================================================================== //


namespace PtclEditor {

enum class TextureAdjustMode {
    None,
    Stretch,
    Fit
};

struct TextureImportSettings {
    TextureAdjustMode mode;
    QSize canvasSize;
    QRect cropRect;
    Qt::TransformationMode filter;
    Ptcl::TextureFormat format;
    ImageUtil::ETC1Quality quality;
    bool dither;
};

class TextureImportProcessor {
public:
    void setSourceImage(QImage image);
    void setSettings(const TextureImportSettings& settings);

    const QImage& adjustedImage() const;

    std::unique_ptr<Ptcl::Texture> buildTexture(const std::atomic<bool>& cancelFlag = ImageUtil::neverCancel()) const;

private:
    QImage mSourceImage;
    QImage mAdjustedImage;
    TextureImportSettings mSettings;
};

} // namespace PtclEditor


// ========================================================================== //
