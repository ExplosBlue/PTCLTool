#pragma once

#include "editor/components/cropWidget.h"
#include "editor/components/enumComboBox.h"
#include "editor/components/loadingSpinner.h"
#include "editor/texture/textureImportProcessor.h"

#include "ptcl/ptclEnum.h"
#include "ptcl/ptclTexture.h"
#include "util/imageUtil.h"

#include <QCheckBox>
#include <QDialog>
#include <QFutureWatcher>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

#include <atomic>


// ========================================================================== //


class TextureImportDialog : public QDialog {
    Q_OBJECT
public:
    explicit TextureImportDialog(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~TextureImportDialog() override;

    void setFilePath(const QString& path);
    std::unique_ptr<Ptcl::Texture> getTexture();

private slots:
    void updateTextureFormat();
    void updateAdjustment();

private:
    void updateImportPreview();
    void updateFormatPreview();
    void updateTextureInfo();
    void populateSizeCombo(QComboBox& combo, s32 initial);

    void updateOkButton();

    PtclEditor::TextureImportSettings buildSettings() const;

    void setupFormatControls();
    void setupAdjustmentControls();
    void setupPreviewWidgets();
    void setupConnections();
    QHBoxLayout* buildImportZoomRow();
    QHBoxLayout* buildFormatZoomRow();

private:
    EnumComboBox<Ptcl::TextureFormat> mFormatSelector{};
    EnumComboBox<ImageUtil::ETC1Quality> mETCQuality{};
    QCheckBox mETCDither{};

    QGroupBox mAdjustGroup{};
    EnumComboBox<PtclEditor::TextureAdjustMode> mAdjustMode{};
    QComboBox mTargetWidth{};
    QComboBox mTargetHeight{};
    EnumComboBox<Qt::TransformationMode> mFilter{};

    CropWidget mImportPreview{};
    ThumbnailWidget mFormatPreview{};
    QPushButton mImportZoomIn{};
    QPushButton mImportZoomOut{};
    QPushButton mImportZoomReset{};
    QPushButton mImportResetCrop{};
    QPushButton mFormatZoomIn{};
    QPushButton mFormatZoomOut{};
    QPushButton mFormatZoomReset{};

    QLabel mInfoLabel{};

    QString mFilePath{};
    QImage mImage{};
    std::unique_ptr<Ptcl::Texture> mTexture{};
    QFutureWatcher<std::unique_ptr<Ptcl::Texture>> mWatcher{};
    LoadingSpinner mLoadingSpinner{};
    PtclEditor::TextureImportProcessor mProcessor{};
    QPushButton* mOkButton{nullptr};
    std::atomic<bool> mCanceled{false};
};


// ========================================================================== //
