#include "editor/inspector/child/childTextureInspector.h"

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


ChildTextureInspector::ChildTextureInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {


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

void ChildTextureInspector::setupConnections() {
    // Wrap T
    connect(&mWrapTComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto wrap = mWrapTComboBox.currentEnum();
        setEmitterProperty(
            "Set Child Texture Wrap U",
            "SetChildTexWrapU",
            &Ptcl::Emitter::childTextureWrapT,
            &Ptcl::Emitter::setChildTextureWrapT,
            wrap
        );
    });

    // Wrap S
    connect(&mWrapSComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto wrap = mWrapSComboBox.currentEnum();
        setEmitterProperty(
            "Set Child Texture Wrap V",
            "SetChildTexWrapV",
            &Ptcl::Emitter::childTextureWrapS,
            &Ptcl::Emitter::setChildTextureWrapS,
            wrap
        );
    });

    // Mag Filter
    connect(&mMagFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto filter = mMagFilterComboBox.currentEnum();
        setEmitterProperty(
            "Set Child Texture Mag Filter",
            "SetChildTexMagFilter",
            &Ptcl::Emitter::childTextureMagFilter,
            &Ptcl::Emitter::setChildTextureMagFilter,
            filter
        );
    });

    // Min Filter
    connect(&mMinFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto filter = mMinFilterComboBox.currentEnum();
        setEmitterProperty(
            "Set Child Texture Min Filter",
            "SetChildTexMinFilter",
            &Ptcl::Emitter::childTextureMinFilter,
            &Ptcl::Emitter::setChildTextureMinFilter,
            filter
        );
    });

    // Mipmap Filter
    connect(&mMipFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto filter = mMipFilterComboBox.currentEnum();
        setEmitterProperty(
            "Set Child Texture Mip Filter",
            "SetChildTexMipFilter",
            &Ptcl::Emitter::childTextureMipFilter,
            &Ptcl::Emitter::setChildTextureMipFilter,
            filter
        );
    });

    // UV Scale
    connect(&mUVScaleSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto scale = mUVScaleSpinBox.getVector();
        setEmitterProperty(
            "Set Child Texture UV Scale",
            "SetChildTexUVScale",
            &Ptcl::Emitter::childTextureUVScale,
            &Ptcl::Emitter::setChildTextureUVScale,
            scale
        );
    });

    // Texture Preview
    connect(&mTexturePreview, &ThumbnailWidget::clicked, this, &ChildTextureInspector::changeTexture);
}

void ChildTextureInspector::populateProperties() {
    QSignalBlocker b1(mWrapTComboBox);
    QSignalBlocker b2(mWrapSComboBox);
    QSignalBlocker b3(mMagFilterComboBox);
    QSignalBlocker b4(mMinFilterComboBox);
    QSignalBlocker b5(mMipFilterComboBox);
    QSignalBlocker b6(mUVScaleSpinBox);

    if (mEmitter->childTextureHandle().isValid()) {
        mTexturePreview.setPixmap(QPixmap::fromImage(mEmitter->childTexture()->textureData()));
    }

    mWrapTComboBox.setCurrentEnum(mEmitter->childTextureWrapT());
    mWrapSComboBox.setCurrentEnum(mEmitter->childTextureWrapS());
    mMagFilterComboBox.setCurrentEnum(mEmitter->childTextureMagFilter());
    mMinFilterComboBox.setCurrentEnum(mEmitter->childTextureMinFilter());
    mMipFilterComboBox.setCurrentEnum(mEmitter->childTextureMipFilter());
    mUVScaleSpinBox.setVector(mEmitter->childTextureUVScale());
}

void ChildTextureInspector::changeTexture() {
    const auto textureList = mDocument->textures();

    TextureSelectDialog dialog(textureList, this);
    if (dialog.exec() == QDialog::Accepted) {
        s32 selectedInded = dialog.selectedIndex();
        if (selectedInded >= 0 && static_cast<size_t>(selectedInded) < textureList.size()) {
            const auto& texture = textureList.at(selectedInded);
            setEmitterProperty(
                "Set Child Texture",
                "SetChildTexture",
                &Ptcl::Emitter::childTexture,
                &Ptcl::Emitter::setChildTexture,
                texture
            );
        }
    }
}


// ========================================================================== //


} // namespace PtclEditor
