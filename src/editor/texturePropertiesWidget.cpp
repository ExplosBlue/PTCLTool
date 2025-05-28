#include "editor/texturePropertiesWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>

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
