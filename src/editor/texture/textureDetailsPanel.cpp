#include "editor/texture/textureDetailsPanel.h"

#include "util/stringUtil.h"

#include <QHBoxLayout>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


TextureDetailsPanel::TextureDetailsPanel(QWidget* parent) :
    QWidget{parent} {
    mThumbnailWidget.setThumbnailSize({256, 256});
    mExportButton.setText("Export");
    mReplaceButton.setText("Replace");
    mDeleteButton.setText("Delete");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addWidget(&mThumbnailWidget);

    QFont boldFont = mNameLabel.font();
    boldFont.setBold(true);
    mNameLabel.setFont(boldFont);

    mainLayout->addWidget(&mNameLabel);
    mainLayout->addWidget(&mFormatLabel);
    mainLayout->addWidget(&mDimensionsLabel);
    mainLayout->addWidget(&mSizeLabel);
    mainLayout->addWidget(&mUsersLabel);

    auto* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(&mExportButton);
    buttonLayout->addWidget(&mReplaceButton);
    buttonLayout->addWidget(&mDeleteButton);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(1);

    connect(&mExportButton, &QPushButton::clicked, this, [this](bool checked) {
        Q_UNUSED(checked);
        if (mTexturePtr) {
            emit exportRequested(mTexturePtr);
        }
    });

    connect(&mReplaceButton, &QPushButton::clicked, this, [this](bool checked) {
        Q_UNUSED(checked);
        if (mIndex.isValid()) {
            emit replaceRequested(mIndex);
        }
    });

    connect(&mDeleteButton, &QPushButton::clicked, this, [this](bool checked) {
        Q_UNUSED(checked);
        if (mIndex.isValid()) {
            emit deleteRequested(mIndex);
        }
    });

    setEnabled(false);
}

void TextureDetailsPanel::setTexture(const QModelIndex& index, Ptcl::Texture* texture) {
    mTexturePtr = texture;
    mIndex = index;

    if (!mTexturePtr) {
        setEnabled(false);
        mThumbnailWidget.clear();
        mNameLabel.clear();
        mFormatLabel.clear();
        mDimensionsLabel.clear();
        mSizeLabel.clear();
        mUsersLabel.clear();
        return;
    }

    setEnabled(true);
    mThumbnailWidget.setPixmap(QPixmap::fromImage(mTexturePtr->textureData()));
    updateLabels();
}

void TextureDetailsPanel::refreshTexture() {
    if (!mTexturePtr) {
        return;
    }

    mThumbnailWidget.setPixmap(QPixmap::fromImage(mTexturePtr->textureData()));
    updateLabels();
}

void TextureDetailsPanel::updateLabels() {
    if (!mTexturePtr) {
        return;
    }

    const auto& img = mTexturePtr->textureData();

    mNameLabel.setText(QString("Texture %1").arg(mIndex.row()));
    mFormatLabel.setText(QString("Format: %1").arg(Ptcl::toString(mTexturePtr->textureFormat())));
    mDimensionsLabel.setText(QString("Dimensions: %1 x %2").arg(img.width()).arg(img.height()));
    mSizeLabel.setText(QString("Size: %1").arg(StringUtil::formatBytes(mTexturePtr->textureDataRaw().size())));
    mUsersLabel.setText(QString("Users: %1").arg(mTexturePtr->userCount()));
}


// ========================================================================== //


} // namespace PtclEditor
