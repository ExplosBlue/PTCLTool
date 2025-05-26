#ifndef TEXTUREIMPORTDIALOG_H
#define TEXTUREIMPORTDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QLabel>
#include <QMovie>

#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

#include "enumComboBox.h"
#include "thumbnailWidget.h"

#include "ptcl/ptclEnum.h"
#include "ptcl/ptclTexture.h"

#include "util/imageUtil.h"

#include "loadingSpinner.h"

class TextureImportDialog : public QDialog {
    Q_OBJECT
public:
    explicit TextureImportDialog(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

    void setFilePath(const QString& path);

    std::unique_ptr<Ptcl::Texture> getTexture();

private slots:
    void updateTextureFormat();

private:
    void updateImportPreview();
    void updateFormatPreview();
    void updateTextureInfo();

private:
    EnumComboBox<Ptcl::TextureFormat> mFormatSelector;
    ThumbnailWidget mImportPreview;
    ThumbnailWidget mFormatPreview;

    EnumComboBox<ImageUtil::ETC1Quality> mETCQuality;
    QCheckBox mETCDither;

    QLabel mInfoLabel;

    QString mFilePath;
    QImage mImage;
    std::unique_ptr<Ptcl::Texture> mTexture;

    QFutureWatcher<std::vector<u8>> mWatcher;
    LoadingSpinner mLoadingSpinner;
};

#endif // TEXTUREIMPORTDIALOG_H
