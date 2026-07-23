#include "editor/inspector/child/childTextureInspector.h"

#include "editor/textureSelectDialog.h"

#include <array>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QGroupBox>


namespace PtclEditor {


// ========================================================================== //


static const std::array textureWrapOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::TextureWrap>{ Ptcl::TextureWrap::MirroredRepeat, "Mirrored Repeat", "Texture is mirrored when repeating." },
    EnumOption<Ptcl::TextureWrap>{ Ptcl::TextureWrap::Repeat,         "Repeat",          "Texture repeats seamlessly." },
    EnumOption<Ptcl::TextureWrap>{ Ptcl::TextureWrap::ClampToEdge,    "Clamp to Edge",   "Texture is clamped at the edges." },
};

static const std::array textureFilterOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::TextureFilter>{ Ptcl::TextureFilter::Linear,  "Linear",  "Smooth interpolation between texels." },
    EnumOption<Ptcl::TextureFilter>{ Ptcl::TextureFilter::Nearest, "Nearest", "Pixelated nearest-neighbor filtering." },
};


ChildTextureInspector::ChildTextureInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {


    mUVScaleSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mTexturePreview.setThumbnailSize(QSize(64, 64));

    // Texture Settings Layout
    auto settingsLayout = new QGridLayout;
    settingsLayout->addWidget(new QLabel("Wrap U:"), 0, 0);
    settingsLayout->addWidget(&mWrapTComboBox, 0, 1);
    mWrapTComboBox.setOptions(textureWrapOptions);
    settingsLayout->addWidget(new QLabel("Wrap V:"), 1, 0);
    settingsLayout->addWidget(&mWrapSComboBox, 1, 1);
    mWrapSComboBox.setOptions(textureWrapOptions);
    settingsLayout->addWidget(new QLabel("LOD Level:"), 0, 2);
    settingsLayout->addWidget(&mTexLodLevel, 0, 3);
    mTexLodLevel.setRange(0, 15);
    mTexLodLevel.setToolTip("Maximum mipmap LOD level (0 = no mipmaps, full resolution only)");
    settingsLayout->addWidget(new QLabel("Filter:"), 1, 2);
    settingsLayout->addWidget(&mFilterComboBox, 1, 3);
    mFilterComboBox.setOptions(textureFilterOptions);
    settingsLayout->addWidget(new QLabel("UV Scale:"), 2, 0);
    settingsLayout->addWidget(&mUVScaleSpinBox, 2, 1);
    mUVScaleSpinBox.setToolTip("Scale factor for child texture UV coordinates");

    settingsLayout->setColumnStretch(0, 0); // labels
    settingsLayout->setColumnStretch(1, 1); // wrap / UV scale widgets
    settingsLayout->setColumnStretch(2, 0); // labels
    settingsLayout->setColumnStretch(3, 1); // filter widgets

    // Main Layout
    auto mainLayout = new QGridLayout(this);
    mainLayout->addLayout(settingsLayout, 0, 0, 1, 1);
    mainLayout->addWidget(&mTexturePreview, 0, 1, 1, 1);
    mainLayout->setColumnStretch(0, 1);

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

    // LOD Level
    connect(&mTexLodLevel, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Child Texture LOD Level",
            "SetChildTexLodLevel",
            &Ptcl::Emitter::childTextureLodLevel,
            &Ptcl::Emitter::setChildTextureLodLevel,
            static_cast<u8>(value)
        );
    });

    // Filter
    connect(&mFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto filter = mFilterComboBox.currentEnum();
        setEmitterProperty(
            "Set Child Texture Filter",
            "SetChildTexFilter",
            &Ptcl::Emitter::childTextureFilter,
            &Ptcl::Emitter::setChildTextureFilter,
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
    QSignalBlocker b3(mTexLodLevel);
    QSignalBlocker b4(mFilterComboBox);
    QSignalBlocker b5(mUVScaleSpinBox);

    if (mEmitter->childTextureHandle().isValid()) {
        mTexturePreview.setPixmap(QPixmap::fromImage(mEmitter->childTexture()->textureData()));
    }

    mWrapTComboBox.setCurrentEnum(mEmitter->childTextureWrapT());
    mWrapSComboBox.setCurrentEnum(mEmitter->childTextureWrapS());
    mTexLodLevel.setValue(mEmitter->childTextureLodLevel());
    mFilterComboBox.setCurrentEnum(mEmitter->childTextureFilter());
    mUVScaleSpinBox.setVector(mEmitter->childTextureUVScale());
}

void ChildTextureInspector::changeTexture() {
    const auto& textureList = mDocument->textures();

    TextureSelectDialog dialog(textureList, this);
    if (dialog.exec() == QDialog::Accepted) {
        s32 selectedInded = dialog.selectedIndex();
        if (selectedInded >= 0 && static_cast<size_t>(selectedInded) < textureList.size()) {
            const auto texture = textureList.at(selectedInded).get();
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
