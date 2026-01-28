#include "editor/childEditor/texturePropertiesWidget.h"

#include "editor/textureSelectDialog.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QGroupBox>


namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::TexturePropertiesWidget::TexturePropertiesWidget(QWidget* parent) :
    QWidget{parent} {


    mUVScaleSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mTexturePreview.setThumbnailSize(QSize(64, 64));

    // Texture Settings Layout
    auto settingsLayout = new QGridLayout;
    settingsLayout->addWidget(new QLabel("Wrap U:"), 0, 0);
    settingsLayout->addWidget(&mWrapTComboBox, 0, 1);
    settingsLayout->addWidget(new QLabel("Wrap V:"), 1, 0);
    settingsLayout->addWidget(&mWrapSComboBox, 1, 1);
    settingsLayout->addWidget(new QLabel("Magnification Filter:"), 0, 2);
    settingsLayout->addWidget(&mMagFilterComboBox, 0, 3);
    settingsLayout->addWidget(new QLabel("Minification Filter:"), 1, 2);
    settingsLayout->addWidget(&mMinFilterComboBox, 1, 3);
    settingsLayout->addWidget(new QLabel("UV Scale:"), 2, 0);
    settingsLayout->addWidget(&mUVScaleSpinBox, 2, 1);
    settingsLayout->addWidget(new QLabel("Mipmap Filter:"), 2, 2);
    settingsLayout->addWidget(&mMipFilterComboBox, 2, 3);

    settingsLayout->setColumnStretch(0, 0); // labels
    settingsLayout->setColumnStretch(1, 1); // wrap / UV scale widgets
    settingsLayout->setColumnStretch(2, 0); // labels
    settingsLayout->setColumnStretch(3, 1); // filter widgets

    // Main Layout
    auto mainLayout = new QGridLayout(this);
    mainLayout->addLayout(settingsLayout, 0, 0, 1, 1);
    mainLayout->addWidget(&mTexturePreview, 0, 1, 1, 1);

    setLayout(mainLayout);
    setupConnections();
}

void ChildEditorWidget::TexturePropertiesWidget::setupConnections() {
    // Wrap T
    connect(&mWrapTComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        mProps.textureWrapT = mWrapTComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    // Wrap S
    connect(&mWrapSComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        mProps.textureWrapS = mWrapSComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    // Mag Filter
    connect(&mMagFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        mProps.textureMagFilter = mMagFilterComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    // Min Filter
    connect(&mMinFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        mProps.textureMinFilter = mMinFilterComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    // Mipmap Filter
    connect(&mMipFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        mProps.textureMipFilter = mMipFilterComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    // UV Scale
    connect(&mUVScaleSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mProps.texUVScale = mUVScaleSpinBox.getVector();
        emit propertiesUpdated(mProps);
    });

    // Texture Preview
    connect(&mTexturePreview, &ThumbnailWidget::clicked, this, &TexturePropertiesWidget::changeTexture);
}

void ChildEditorWidget::TexturePropertiesWidget::setProperties(const Ptcl::ChildData::TextureProperties& properties, const std::shared_ptr<Ptcl::Texture>& texture) {
    QSignalBlocker b1(mWrapTComboBox);
    QSignalBlocker b2(mWrapSComboBox);
    QSignalBlocker b3(mMagFilterComboBox);
    QSignalBlocker b4(mMinFilterComboBox);
    QSignalBlocker b5(mMipFilterComboBox);
    QSignalBlocker b6(mUVScaleSpinBox);

    mProps = properties;
    mTexture = texture;

    if (mTexture) {
        mTexturePreview.setPixmap(QPixmap::fromImage(mTexture->textureData()));
    }

    mWrapTComboBox.setCurrentEnum(mProps.textureWrapT);
    mWrapSComboBox.setCurrentEnum(mProps.textureWrapS);
    mMagFilterComboBox.setCurrentEnum(mProps.textureMagFilter);
    mMinFilterComboBox.setCurrentEnum(mProps.textureMinFilter);
    mMipFilterComboBox.setCurrentEnum(mProps.textureMipFilter);
    mUVScaleSpinBox.setVector(mProps.texUVScale);
}

void ChildEditorWidget::TexturePropertiesWidget::setTextureList(const Ptcl::TextureList* textureList) {
    mTextureList = textureList;
}

void ChildEditorWidget::TexturePropertiesWidget::changeTexture() {
    if (!mTextureList) {
        return;
    }

    auto oldTexture = mTexture;

    TextureSelectDialog dialog(*mTextureList, this);
    if (dialog.exec() == QDialog::Accepted) {
        s32 selectedInded = dialog.selectedIndex();
        if (selectedInded >= 0 && static_cast<size_t>(selectedInded) < mTextureList->size()) {
            mTexture = mTextureList->at(selectedInded);
            mTexturePreview.setPixmap(QPixmap::fromImage(mTexture->textureData()));
            emit textureUpdated(oldTexture, mTexture);
        }
    }
}


// ========================================================================== //


} // namespace PtclEditor
