#include "editor/inspector/child/childTextureInspector.h"

#include "editor/texture/textureSelectDialog.h"

#include <array>
#include <cmath>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>


namespace PtclEditor {


// ==========================================================================//


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

    mDivXSpinBox.setRange(1, 16);
    mDivYSpinBox.setRange(1, 16);
    mRepXSpinBox.setRange(1, 256);
    mRepYSpinBox.setRange(1, 256);

    mTexturePreview.showFrameNumbers(false);
    mTexturePreview.setGreyFrameThreshold(1);

    // Texture Settings
    auto settingsLayout = new QGridLayout;
    settingsLayout->addWidget(new QLabel("Wrap U:"), 0, 0);
    settingsLayout->addWidget(&mWrapTComboBox, 0, 1);
    mWrapTComboBox.setOptions(textureWrapOptions);
    mWrapTComboBox.setDescription("How the texture repeats horizontally.");
    settingsLayout->addWidget(new QLabel("Wrap V:"), 1, 0);
    settingsLayout->addWidget(&mWrapSComboBox, 1, 1);
    mWrapSComboBox.setOptions(textureWrapOptions);
    mWrapSComboBox.setDescription("How the texture repeats vertically.");
    settingsLayout->addWidget(new QLabel("LOD Level:"), 0, 2);
    settingsLayout->addWidget(&mTexLodLevel, 0, 3);
    mTexLodLevel.setRange(0, 15);
    mTexLodLevel.setToolTip("Maximum mipmap LOD level (0 = no mipmaps, full resolution only).");
    settingsLayout->addWidget(new QLabel("Filter:"), 1, 2);
    settingsLayout->addWidget(&mFilterComboBox, 1, 3);
    mFilterComboBox.setOptions(textureFilterOptions);
    mFilterComboBox.setDescription("Texture filtering method when scaling.");
    settingsLayout->setColumnStretch(1, 1);
    settingsLayout->setColumnStretch(3, 1);

    auto divRepLayout = new QGridLayout;
    divRepLayout->addWidget(new QLabel("Divisions:"), 0, 0);
    divRepLayout->addWidget(&mDivXSpinBox, 0, 1);
    divRepLayout->addWidget(&mDivYSpinBox, 0, 2);
    divRepLayout->addWidget(new QLabel("Repetitions:"), 1, 0);
    divRepLayout->addWidget(&mRepXSpinBox, 1, 1);
    divRepLayout->addWidget(&mRepYSpinBox, 1, 2);
    mDivXSpinBox.setToolTip("Number of horizontal divisions in the texture (only the first frame is used).");
    mDivYSpinBox.setToolTip("Number of vertical divisions in the texture (only the first frame is used).");
    mRepXSpinBox.setToolTip("Number of horizontal texture repetitions.");
    mRepYSpinBox.setToolTip("Number of vertical texture repetitions.");

    mUVLabel = new QLabel(this);
    mUVLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    mChangeTextureButton.setText("Change Texture");
    mChangeTextureButton.setToolTip("Choose a different texture from the document.");

    // Right side controls
    auto rightLayout = new QVBoxLayout;
    rightLayout->addLayout(settingsLayout);
    rightLayout->addLayout(divRepLayout);
    rightLayout->addWidget(mUVLabel);
    rightLayout->addWidget(&mChangeTextureButton);
    rightLayout->addStretch();

    // Left side canvas
    auto leftLayout = new QVBoxLayout;
    leftLayout->addWidget(&mTexturePreview, 0, Qt::AlignTop);

    // Canvas + Controls side by side
    auto topRow = new QHBoxLayout;
    topRow->addLayout(leftLayout, 1);
    topRow->addLayout(rightLayout);

    // Main Layout
    auto mainLayout = new QVBoxLayout(this);
    addSectionHeader(mainLayout, "Texture Mapping", this);
    mainLayout->addLayout(topRow, 1);

    setupConnections();
}

void ChildTextureInspector::updateUVLabel() {
    const s32 repX = mRepXSpinBox.value();
    const s32 repY = mRepYSpinBox.value();
    const s32 divX = std::max(1, static_cast<s32>(mDivXSpinBox.value()));
    const s32 divY = std::max(1, static_cast<s32>(mDivYSpinBox.value()));
    const f32 uvX = static_cast<f32>(repX) / static_cast<f32>(divX);
    const f32 uvY = static_cast<f32>(repY) / static_cast<f32>(divY);

    mUVLabel->setText(
        QString("UV Scale: %1 x %2")
            .arg(uvX, 0, 'g', 3)
            .arg(uvY, 0, 'g', 3)
    );
}

void ChildTextureInspector::setupConnections() {
    connect(&mChangeTextureButton, &QPushButton::clicked, this, &ChildTextureInspector::changeTexture);

    // Wrap T
    connect(&mWrapTComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto wrap = mWrapTComboBox.currentEnum();
        mTexturePreview.setWrapModes(wrap, mWrapSComboBox.currentEnum());

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
        mTexturePreview.setWrapModes(mWrapTComboBox.currentEnum(), wrap);

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
        mTexturePreview.setFilter(filter);

        setEmitterProperty(
            "Set Child Texture Filter",
            "SetChildTexFilter",
            &Ptcl::Emitter::childTextureFilter,
            &Ptcl::Emitter::setChildTextureFilter,
            filter
        );
    });

    // Division X
    connect(&mDivXSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        const s32 divX = static_cast<s32>(value);
        const s32 maxY = 16 / std::max(1, divX);

        QSignalBlocker by(mDivYSpinBox);
        mDivYSpinBox.setMaximum(maxY);
        const s32 divY = std::clamp(static_cast<s32>(mDivYSpinBox.value()), 1, maxY);
        mDivYSpinBox.setValue(divY);

        const s32 repX = mRepXSpinBox.value();
        const s32 repY = mRepYSpinBox.value();

        mTexturePreview.setDivisions(divX, divY);
        mTexturePreview.setRepetitions(repX, repY);
        updateUVLabel();

        mUpdatingEmitter = true;
        setEmitterProperty(
            "Set Child Texture Divisions",
            "SetChildTexDiv",
            &Ptcl::Emitter::childTextureUVScale,
            &Ptcl::Emitter::setChildTextureUVScale,
            Math::Vector2f(
                static_cast<f32>(repX) / static_cast<f32>(divX),
                static_cast<f32>(repY) / static_cast<f32>(divY)
            )
        );
        mUpdatingEmitter = false;
    });

    // Division Y
    connect(&mDivYSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        const s32 divY = static_cast<s32>(value);
        const s32 maxX = 16 / std::max(1, divY);

        QSignalBlocker bx(mDivXSpinBox);
        mDivXSpinBox.setMaximum(maxX);
        const s32 divX = std::clamp(static_cast<s32>(mDivXSpinBox.value()), 1, maxX);
        mDivXSpinBox.setValue(divX);

        const s32 repX = mRepXSpinBox.value();
        const s32 repY = mRepYSpinBox.value();

        mTexturePreview.setDivisions(divX, divY);
        mTexturePreview.setRepetitions(repX, repY);
        updateUVLabel();

        mUpdatingEmitter = true;
        setEmitterProperty(
            "Set Child Texture Divisions",
            "SetChildTexDiv",
            &Ptcl::Emitter::childTextureUVScale,
            &Ptcl::Emitter::setChildTextureUVScale,
            Math::Vector2f(
                static_cast<f32>(repX) / static_cast<f32>(divX),
                static_cast<f32>(repY) / static_cast<f32>(divY)
            )
        );
        mUpdatingEmitter = false;
    });

    // Repetitions X
    connect(&mRepXSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        const s32 repX = static_cast<s32>(value);
        const s32 repY = mRepYSpinBox.value();
        const s32 divX = mDivXSpinBox.value();
        const s32 divY = mDivYSpinBox.value();

        mTexturePreview.setRepetitions(repX, repY);
        updateUVLabel();

        mUpdatingEmitter = true;
        setEmitterProperty(
            "Set Child Texture Repetitions X",
            "SetChildTexRepX",
            &Ptcl::Emitter::childTextureUVScale,
            &Ptcl::Emitter::setChildTextureUVScale,
            Math::Vector2f(
                static_cast<f32>(repX) / static_cast<f32>(divX),
                static_cast<f32>(repY) / static_cast<f32>(divY)
            )
        );
        mUpdatingEmitter = false;
    });

    // Repetitions Y
    connect(&mRepYSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        const s32 repX = mRepXSpinBox.value();
        const s32 repY = static_cast<s32>(value);
        const s32 divX = mDivXSpinBox.value();
        const s32 divY = mDivYSpinBox.value();

        mTexturePreview.setRepetitions(repX, repY);
        updateUVLabel();

        mUpdatingEmitter = true;
        setEmitterProperty(
            "Set Child Texture Repetitions Y",
            "SetChildTexRepY",
            &Ptcl::Emitter::childTextureUVScale,
            &Ptcl::Emitter::setChildTextureUVScale,
            Math::Vector2f(
                static_cast<f32>(repX) / static_cast<f32>(divX),
                static_cast<f32>(repY) / static_cast<f32>(divY)
            )
        );
        mUpdatingEmitter = false;
    });
}

void ChildTextureInspector::populateProperties() {
    if (mUpdatingEmitter) {
        return;
    }

    QSignalBlocker b1(mWrapTComboBox);
    QSignalBlocker b2(mWrapSComboBox);
    QSignalBlocker b3(mTexLodLevel);
    QSignalBlocker b4(mFilterComboBox);
    QSignalBlocker b5(mDivXSpinBox);
    QSignalBlocker b6(mDivYSpinBox);
    QSignalBlocker b7(mRepXSpinBox);
    QSignalBlocker b8(mRepYSpinBox);

    if (mEmitter->childTextureHandle().isValid()) {
        const auto& texData = mEmitter->childTexture()->textureData();
        const auto uvScale = mEmitter->childTextureUVScale();

        // Find best div/rep pair that matches the stored UV scale.
        // For each axis, pick smallest div (1-16) where rep = uv * div is close to integer.
        auto unpackUV = [](f32 uv, s32& div, s32& rep) {
            for (s32 d = 1; d <= 16; ++d) {
                const f32 raw = uv * static_cast<f32>(d);
                const s32 rounded = static_cast<s32>(std::round(raw));
                if (rounded >= 1 && rounded <= 256 && std::abs(raw - static_cast<f32>(rounded)) < 0.01f) {
                    div = d;
                    rep = rounded;
                    return;
                }
            }
            div = 1;
            rep = std::clamp(static_cast<s32>(std::round(uv)), 1, 256);
        };

        s32 divX = 1, repX = 1;
        s32 divY = 1, repY = 1;
        unpackUV(uvScale.getX(), divX, repX);
        unpackUV(uvScale.getY(), divY, repY);

        mDivXSpinBox.setValue(static_cast<u8>(divX));
        mDivYSpinBox.setValue(static_cast<u8>(divY));
        mRepXSpinBox.setValue(static_cast<u16>(repX));
        mRepYSpinBox.setValue(static_cast<u16>(repY));

        mTexturePreview.setSource(texData, divX, divY);
        mTexturePreview.setWrapModes(mEmitter->childTextureWrapT(), mEmitter->childTextureWrapS());
        mTexturePreview.setFilter(mEmitter->childTextureFilter());
        mTexturePreview.setRepetitions(repX, repY);

        updateUVLabel();
    }

    mWrapTComboBox.setCurrentEnum(mEmitter->childTextureWrapT());
    mWrapSComboBox.setCurrentEnum(mEmitter->childTextureWrapS());
    mTexLodLevel.setValue(mEmitter->childTextureLodLevel());
    mFilterComboBox.setCurrentEnum(mEmitter->childTextureFilter());
}

void ChildTextureInspector::changeTexture() {
    const auto& textureList = mDocument->textures();

    TextureSelectDialog dialog(textureList, this);
    if (dialog.exec() == QDialog::Accepted) {
        s32 selectedInded = dialog.selectedIndex();
        if (selectedInded >= 0 && static_cast<size_t>(selectedInded) < textureList.size()) {
            const auto texture = textureList.at(selectedInded).get();
            mUpdatingEmitter = true;
            setEmitterProperty(
                "Set Child Texture",
                "SetChildTexture",
                &Ptcl::Emitter::childTexture,
                &Ptcl::Emitter::setChildTexture,
                texture
            );
            mUpdatingEmitter = false;
        }
    }
}


// ==========================================================================//


} // namespace PtclEditor
