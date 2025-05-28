#include "editor/texturePropertiesWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

#include "editor/textureSelectDialog.h"

TexturePropertiesWidget::TexturePropertiesWidget(QWidget* parent) :
    QWidget{parent},
    mEmitterPtr{nullptr} {

    // Wrap T
    connect(&mWrapTComboBox, &QComboBox::currentIndexChanged, this, &TexturePropertiesWidget::updateWrapT);

    // Wrap S
    connect(&mWrapSComboBox, &QComboBox::currentIndexChanged, this, &TexturePropertiesWidget::updateWrapS);

    // Mag Filter
    connect(&mMagFilterComboBox, &QComboBox::currentIndexChanged, this, &TexturePropertiesWidget::updateMagFilter);

    // Min Filter
    connect(&mMinFilterComboBox, &QComboBox::currentIndexChanged, this, &TexturePropertiesWidget::updateMinFilter);

    // Mipmap Filter
    connect(&mMipFilterComboBox, &QComboBox::currentIndexChanged, this, &TexturePropertiesWidget::updateMipFilter);

    // Texture Preview
    connect(&mTexturePreview, &ThumbnailWidget::clicked, this, &TexturePropertiesWidget::changeTexture);
    mTexturePreview.setThumbnailSize(QSize(64, 64));

    // Texture Settings Layout
    auto settingsLayout = new QGridLayout;
    settingsLayout->addWidget(new QLabel("Wrap T:"), 0, 0);
    settingsLayout->addWidget(&mWrapTComboBox, 0, 1);
    settingsLayout->addWidget(new QLabel("Wrap S:"), 1, 0);
    settingsLayout->addWidget(&mWrapSComboBox, 1, 1);
    settingsLayout->addWidget(new QLabel("Magnification Filter:"), 0, 2);
    settingsLayout->addWidget(&mMagFilterComboBox, 0, 3);
    settingsLayout->addWidget(new QLabel("Minification Filter:"), 1, 2);
    settingsLayout->addWidget(&mMinFilterComboBox, 1, 3);
    settingsLayout->addWidget(new QLabel("Mipmap Filter:"), 2, 2);
    settingsLayout->addWidget(&mMipFilterComboBox, 2, 3);

    // Main Layout
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(&mTexturePreview);
    mainLayout->addLayout(settingsLayout);
    setLayout(mainLayout);
}

void TexturePropertiesWidget::setEmitter(Ptcl::Emitter* emitter) {
    mEmitterPtr = emitter;

    populateWidgets();
}

void TexturePropertiesWidget::populateWidgets() {
    if (!mEmitterPtr) {
        return;
    }

    if (mEmitterPtr->textureHandle().isValid()) {
        mTexturePreview.setPixmap(QPixmap::fromImage(mEmitterPtr->textureHandle()->textureData()));
    }

    mWrapTComboBox.setCurrentEnum(mEmitterPtr->textureWrapT());
    mWrapSComboBox.setCurrentEnum(mEmitterPtr->textureWrapS());
    mMagFilterComboBox.setCurrentEnum(mEmitterPtr->textureMagFilter());
    mMinFilterComboBox.setCurrentEnum(mEmitterPtr->textureMinFilter());
    mMipFilterComboBox.setCurrentEnum(mEmitterPtr->textureMipFilter());
}

void TexturePropertiesWidget::setTextureList(const Ptcl::TextureList* textureList) {
    mTextureList = textureList;
}

void TexturePropertiesWidget::updateWrapT() {
    if (!mEmitterPtr) {
        return;
    }

    mEmitterPtr->setTextureWrapT(mWrapTComboBox.currentEnum());
}

void TexturePropertiesWidget::updateWrapS() {
    if (!mEmitterPtr) {
        return;
    }

    mEmitterPtr->setTextureWrapS(mWrapSComboBox.currentEnum());
}

void TexturePropertiesWidget::updateMinFilter() {
    if (!mEmitterPtr) {
        return;
    }

    mEmitterPtr->setTextureMinFilter(mMinFilterComboBox.currentEnum());
}

void TexturePropertiesWidget::updateMagFilter() {
    if (!mEmitterPtr) {
        return;
    }

    mEmitterPtr->setTextureMagFilter(mMagFilterComboBox.currentEnum());
}

void TexturePropertiesWidget::updateMipFilter() {
    if (!mEmitterPtr) {
        return;
    }

    mEmitterPtr->setTextureMipFilter(mMipFilterComboBox.currentEnum());
}

void TexturePropertiesWidget::updateTextureDetails() {
    mTexturePreview.setPixmap(QPixmap::fromImage(mEmitterPtr->textureHandle()->textureData()));
}

void TexturePropertiesWidget::changeTexture() {
    if (!mEmitterPtr || !mTextureList) {
        return;
    }

    auto oldTexture = mEmitterPtr->textureHandle().get();
    int oldIndex = -1;

    for (size_t i = 0; i < mTextureList->size(); ++i) {
        if (mTextureList->at(i) == oldTexture) {
            oldIndex = static_cast<int>(i);
            break;
        }
    }

    TextureSelectDialog dialog(*mTextureList, this);
    if (dialog.exec() == QDialog::Accepted) {
        int index = dialog.selectedIndex();
        if (index >= 0 && static_cast<size_t>(index) < mTextureList->size()) {
            mEmitterPtr->textureHandle().set(mTextureList->at(index));
            updateTextureDetails();
            emit textureUpdated(oldIndex, index);
        }
    }
}
