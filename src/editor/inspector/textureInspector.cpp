#include "editor/inspector/textureInspector.h"

#include "editor/textureSelectDialog.h"

#include "util/paintUtil.h"

#include <array>

#include <QApplication>
#include <QGuiApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QResizeEvent>
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

static const std::array animModeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<FrameAnimMode>{ FrameAnimMode::FixedSpeed, "Fixed Rate", "Frames advance at a fixed interval, independent of particle lifetime." },
    EnumOption<FrameAnimMode>{ FrameAnimMode::FitToLife,  "Fit to Life", "Frames are distributed evenly across the particle's lifetime." },
};


// ========================================================================== //


TextureInspector::TextureInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    mTexPatFreq.setMinimum(1);
    mAnimFrameCountSpinBox.setRange(2, 16);
    mChangeTextureButton.setText("Change Texture");

    mFrameTimeline.setFrames(std::vector<s32>(16, 0), 2);

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

    // Division and Repetition Spin Boxes
    mDivXSpinBox.setRange(1, 16);
    mDivYSpinBox.setRange(1, 16);
    mRepXSpinBox.setRange(1, 64);
    mRepYSpinBox.setRange(1, 64);

    mDivXSpinBox.setToolTip("Number of horizontal divisions (animation frames across).");
    mDivYSpinBox.setToolTip("Number of vertical divisions (animation frames down).");
    mRepXSpinBox.setToolTip("Number of horizontal texture repetitions.");
    mRepYSpinBox.setToolTip("Number of vertical texture repetitions.");

    auto divRepLayout = new QGridLayout;
    divRepLayout->addWidget(new QLabel("Divisions:"), 0, 0);
    divRepLayout->addWidget(&mDivXSpinBox, 0, 1);
    divRepLayout->addWidget(&mDivYSpinBox, 0, 2);
    divRepLayout->addWidget(new QLabel("Repetitions:"), 1, 0);
    divRepLayout->addWidget(&mRepXSpinBox, 1, 1);
    divRepLayout->addWidget(&mRepYSpinBox, 1, 2);

    mDescriptionLabel = new QLabel(this);
    mDescriptionLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Right side controls
    auto rightLayout = new QVBoxLayout;
    rightLayout->addLayout(settingsLayout);
    rightLayout->addLayout(divRepLayout);
    rightLayout->addWidget(mDescriptionLabel);
    rightLayout->addWidget(&mChangeTextureButton);
    mChangeTextureButton.setToolTip("Choose a different texture from the document.");
    mStartFrameList.setToolTip("Number of possible spawning frames.\nRandomly selected when a particle spawns.");
    rightLayout->addWidget(&mStartFrameList);
    rightLayout->addStretch();

    // Left side canvas
    auto leftLayout = new QVBoxLayout;
    leftLayout->addWidget(&mTexturePreview, 1);

    // Canvas + Controls side by side
    auto topRow = new QHBoxLayout;
    topRow->addLayout(leftLayout, 1);
    topRow->addLayout(rightLayout);

    // Animation
    mAnimModeComboBox.setOptions(animModeOptions);
    mAnimModeComboBox.setDescription("How animation frames are distributed over time.");



    auto texPatSettingsLayout = new QVBoxLayout;
    texPatSettingsLayout->setSpacing(4);

    auto controlsRow = new QHBoxLayout;
    controlsRow->addWidget(new QLabel("Mode:"));
    controlsRow->addWidget(&mAnimModeComboBox);
    controlsRow->addSpacing(8);
    controlsRow->addWidget(new QLabel("Interval:"));
    controlsRow->addWidget(&mTexPatFreq);
    mTexPatFreq.setToolTip("Ticks between animation frames (Fixed Rate only).");
    controlsRow->addSpacing(8);
    controlsRow->addWidget(new QLabel("Frames:"));
    controlsRow->addWidget(&mAnimFrameCountSpinBox);
    mAnimFrameCountSpinBox.setToolTip("Number of frames in the animation sequence.");

    mFramesWarningLabel = new QLabel(this);
    mFramesWarningLabel->setText("Frame count exceeds lifespan");
    mFramesWarningLabel->setStyleSheet("QLabel { color: #cc8800; font-weight: bold; }");
    mFramesWarningLabel->setVisible(false);
    controlsRow->addWidget(mFramesWarningLabel);
    controlsRow->addStretch();

    texPatSettingsLayout->addLayout(controlsRow);
    texPatSettingsLayout->addWidget(&mFrameTimeline);

    mTexPatGroupBox.setLayout(texPatSettingsLayout);

    // Main Layout
    auto mainLayout = new QVBoxLayout(this);

    addSectionHeader(mainLayout, "Texture Mapping", this);
    mainLayout->addLayout(topRow, 1);

    addSectionHeader(mainLayout, "Texture Animation", this);
    mTexPatAnimCheckBox.setText("Enable Animation");
    mTexPatAnimCheckBox.setToolTip("Enable frame-by-frame texture animation.");
    mainLayout->addWidget(&mTexPatAnimCheckBox);
    mainLayout->addWidget(&mTexPatGroupBox);

    setLayout(mainLayout);
    setupConnections();
}

void TextureInspector::setupConnections() {
    connect(&mChangeTextureButton, &QPushButton::clicked, this, &TextureInspector::changeTexture);

    // Wrap T
    connect(&mWrapTComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto wrap = mWrapTComboBox.currentEnum();
        mTexturePreview.setWrapModes(wrap, mWrapSComboBox.currentEnum());

        if (mEmitter->textureHandle().isValid()) {
            const auto& texData = mEmitter->textureHandle()->textureData();
            const TextureFrameInfo frameInfo{
                .texture = &texData,
                .wrapU = wrap,
                .wrapV = mWrapSComboBox.currentEnum(),
                .divX = mEmitter->numTextureDivisionX(),
                .divY = mEmitter->numTextureDivisionY(),
                .repX = mEmitter->numTextureRepetitionsX(),
                .repY = mEmitter->numTextureRepetitionsY(),
            };
            mFrameTimeline.setSource(frameInfo);
            mStartFrameList.setSource(frameInfo);
        }

        setEmitterProperty(
            "Set Texture Wrap U",
            "SetTexWrapU",
            &Ptcl::Emitter::textureWrapT,
            &Ptcl::Emitter::setTextureWrapT,
            wrap
        );
    });

    // Wrap S
    connect(&mWrapSComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto wrap = mWrapSComboBox.currentEnum();
        mTexturePreview.setWrapModes(mWrapTComboBox.currentEnum(), wrap);

        if (mEmitter->textureHandle().isValid()) {
            const auto& texData = mEmitter->textureHandle()->textureData();
            const TextureFrameInfo frameInfo{
                .texture = &texData,
                .wrapU = mWrapTComboBox.currentEnum(),
                .wrapV = wrap,
                .divX = mEmitter->numTextureDivisionX(),
                .divY = mEmitter->numTextureDivisionY(),
                .repX = mEmitter->numTextureRepetitionsX(),
                .repY = mEmitter->numTextureRepetitionsY(),
            };
            mFrameTimeline.setSource(frameInfo);
            mStartFrameList.setSource(frameInfo);
        }

        setEmitterProperty(
            "Set Texture Wrap V",
            "SetTexWrapV",
            &Ptcl::Emitter::textureWrapS,
            &Ptcl::Emitter::setTextureWrapS,
            wrap
        );
    });

    // LOD Level
    connect(&mTexLodLevel, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture LOD Level",
            "SetTexLodLevel",
            &Ptcl::Emitter::textureLodLevel,
            &Ptcl::Emitter::setTextureLodLevel,
            static_cast<u8>(value)
        );
    });

    // Filter
    connect(&mFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto filter = mFilterComboBox.currentEnum();
        mTexturePreview.setFilter(filter);

        if (mEmitter->textureHandle().isValid()) {
            const auto& texData = mEmitter->textureHandle()->textureData();
            const TextureFrameInfo frameInfo{
                .texture = &texData,
                .wrapU = mWrapTComboBox.currentEnum(),
                .wrapV = mWrapSComboBox.currentEnum(),
                .filter = filter,
                .divX = mEmitter->numTextureDivisionX(),
                .divY = mEmitter->numTextureDivisionY(),
                .repX = mEmitter->numTextureRepetitionsX(),
                .repY = mEmitter->numTextureRepetitionsY(),
            };
            mFrameTimeline.setSource(frameInfo);
            mStartFrameList.setSource(frameInfo);
        }

        setEmitterProperty(
            "Set Texture Filter",
            "SetTexFilter",
            &Ptcl::Emitter::textureFilter,
            &Ptcl::Emitter::setTextureFilter,
            filter
        );
    });

    // Texture pattern count
    connect(&mStartFrameList, &StartFrameList::slotCountChanged, this, [this](u16 value) {
        setEmitterProperty(
            "Changed Start Frame Count",
            "SetNumTexPat",
            &Ptcl::Emitter::numTexturePattern,
            &Ptcl::Emitter::setNumTexturePattern,
            value
        );
    });

    connect(&mTexPatFreq, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mFrameTimeline.setFrameStep(static_cast<s32>(value));
        setEmitterProperty(
            "Set Texture Anim Frame Step",
            "SetTexPatFreq",
            &Ptcl::Emitter::texturePatternFrequency,
            &Ptcl::Emitter::setTexturePatternFrequency,
            static_cast<u16>(value)
        );
    });

    connect(&mAnimFrameCountSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture Anim Frame Count",
            "SetTexPatFrameCount",
            &Ptcl::Emitter::texturePatternFrameCount,
            &Ptcl::Emitter::setTexturePatternFrameCount,
            static_cast<u16>(value)
        );
    });

    connect(&mFrameTimeline, &FrameTimeline::frameEdited, this, [this](s32 index, s32 value) {
        auto table = mEmitter->texturePatternTable();
        table[index] = static_cast<u8>(value);

        setEmitterProperty(
            "Set Texture Anim Frame Order",
            QString("SetTexPatTbl_%1").arg(index),
            &Ptcl::Emitter::texturePatternTable,
            &Ptcl::Emitter::setTexturePatternTable,
            table
        );
    });


    connect(&mAnimModeComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto mode = mAnimModeComboBox.currentEnum();
        mFrameTimeline.setAnimMode(mode);

        u16 freq;
        if (mode == FrameAnimMode::FitToLife) {
            freq = 0;
        } else {
            freq = 1;
        }

        setEmitterProperty(
            "Set Texture Anim Mode",
            "SetTexAnimMode",
            &Ptcl::Emitter::texturePatternFrequency,
            &Ptcl::Emitter::setTexturePatternFrequency,
            freq
        );
    });

    connect(&mTexPatAnimCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        mTexPatGroupBox.setVisible(checked);
        setEmitterProperty(
            "Toggle Texture Anim",
            "ToggleTexAnim",
            &Ptcl::Emitter::isTexturePatternAnim,
            &Ptcl::Emitter::setIsTexturePatternAnim,
            checked
        );
    });

    // Division spin boxes
    connect(&mDivXSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        const s32 divX = static_cast<s32>(value);
        const s32 maxY = 16 / std::max(1, divX);

        QSignalBlocker by(mDivYSpinBox);
        mDivYSpinBox.setMaximum(maxY);
        const s32 divY = std::clamp(static_cast<s32>(mDivYSpinBox.value()), 1, maxY);
        mDivYSpinBox.setValue(divY);

        mTexturePreview.setDivisions(divX, divY);

        mDocument->undoStack()->beginMacro(formatHistoryLabel("Set Texture Divisions"));
        setEmitterProperty(
            "Set Texture Divisions X",
            "SetTexDivX",
            &Ptcl::Emitter::numTextureDivisionX,
            &Ptcl::Emitter::setNumTextureDivisionX,
            static_cast<u8>(divX)
            );
        setEmitterProperty(
            "Set Texture Divisions Y",
            "SetTexDivY",
            &Ptcl::Emitter::numTextureDivisionY,
            &Ptcl::Emitter::setNumTextureDivisionY,
            static_cast<u8>(divY)
            );
        mDocument->undoStack()->endMacro();

        mStartFrameList.setSlotMax(divX * divY);
        updateDescription();
    });

    connect(&mDivYSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        const s32 divY = static_cast<s32>(value);
        const s32 maxX = 16 / std::max(1, divY);

        QSignalBlocker bx(mDivXSpinBox);
        mDivXSpinBox.setMaximum(maxX);
        const s32 divX = std::clamp(static_cast<s32>(mDivXSpinBox.value()), 1, maxX);
        mDivXSpinBox.setValue(divX);

        mTexturePreview.setDivisions(divX, divY);

        mDocument->undoStack()->beginMacro(formatHistoryLabel("Set Texture Divisions"));
        setEmitterProperty(
            "Set Texture Divisions X",
            "SetTexDivX",
            &Ptcl::Emitter::numTextureDivisionX,
            &Ptcl::Emitter::setNumTextureDivisionX,
            static_cast<u8>(divX)
            );
        setEmitterProperty(
            "Set Texture Divisions Y",
            "SetTexDivY",
            &Ptcl::Emitter::numTextureDivisionY,
            &Ptcl::Emitter::setNumTextureDivisionY,
            static_cast<u8>(divY)
            );
        mDocument->undoStack()->endMacro();

        mStartFrameList.setSlotMax(divX * divY);
        updateDescription();
    });

    // Repetition spin boxes
    connect(&mRepXSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        const s32 repX = static_cast<s32>(value);
        const s32 repY = mRepYSpinBox.value();

        mTexturePreview.setRepetitions(repX, repY);

        setEmitterProperty(
            "Set Texture Repetition X",
            "SetTexRepX",
            &Ptcl::Emitter::numTextureRepetitionsX,
            &Ptcl::Emitter::setNumTextureRepetitionsX,
            repX
        );
        updateDescription();
    });

    connect(&mRepYSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        const s32 repX = mRepXSpinBox.value();
        const s32 repY = static_cast<s32>(value);

        mTexturePreview.setRepetitions(repX, repY);

        setEmitterProperty(
            "Set Texture Repetition Y",
            "SetTexRepY",
            &Ptcl::Emitter::numTextureRepetitionsY,
            &Ptcl::Emitter::setNumTextureRepetitionsY,
            repY
        );
        updateDescription();
    });
}

void TextureInspector::populateProperties() {
    QSignalBlocker b1(mWrapTComboBox);
    QSignalBlocker b2(mWrapSComboBox);
    QSignalBlocker b3(mTexLodLevel);
    QSignalBlocker b4(mFilterComboBox);
    QSignalBlocker b5(mTexPatFreq);
    QSignalBlocker b6(mAnimFrameCountSpinBox);
    QSignalBlocker b7(mTexPatAnimCheckBox);
    QSignalBlocker b8(mAnimModeComboBox);
    QSignalBlocker b9(mDivXSpinBox);
    QSignalBlocker b10(mDivYSpinBox);
    QSignalBlocker b11(mRepXSpinBox);
    QSignalBlocker b12(mRepYSpinBox);

    const auto& texData = mEmitter->textureHandle()->textureData();
    mTexturePreview.setSource(texData, mEmitter->numTextureDivisionX(), mEmitter->numTextureDivisionY());
    mTexturePreview.setRepetitions(mEmitter->numTextureRepetitionsX(), mEmitter->numTextureRepetitionsY());

    const auto wrapT = mEmitter->textureWrapT();
    const auto wrapS = mEmitter->textureWrapS();
    const auto filter = mEmitter->textureFilter();

    const TextureFrameInfo frameInfo{
        .texture = &texData,
        .wrapU = wrapT,
        .wrapV = wrapS,
        .filter = filter,
        .divX = mEmitter->numTextureDivisionX(),
        .divY = mEmitter->numTextureDivisionY(),
        .repX = mEmitter->numTextureRepetitionsX(),
        .repY = mEmitter->numTextureRepetitionsY(),
    };
    mFrameTimeline.setSource(frameInfo);

    mWrapTComboBox.setCurrentEnum(wrapT);
    mWrapSComboBox.setCurrentEnum(wrapS);
    mTexturePreview.setWrapModes(wrapT, wrapS);
    mTexturePreview.setFilter(filter);
    mTexLodLevel.setValue(mEmitter->textureLodLevel());
    mFilterComboBox.setCurrentEnum(filter);

    // Populate division and repetition spin boxes
    const s32 divX = mEmitter->numTextureDivisionX();
    const s32 divY = mEmitter->numTextureDivisionY();
    mDivXSpinBox.setMaximum(16 / std::max(1, divY));
    mDivYSpinBox.setMaximum(16 / std::max(1, divX));
    mDivXSpinBox.setValue(static_cast<u8>(divX));
    mDivYSpinBox.setValue(static_cast<u8>(divY));
    mRepXSpinBox.setValue(static_cast<u16>(mEmitter->numTextureRepetitionsX()));
    mRepYSpinBox.setValue(static_cast<u16>(mEmitter->numTextureRepetitionsY()));

    updateDescription();

    // Populate variant slots
    mStartFrameList.setSource(frameInfo);
    mStartFrameList.setSlotMax(maxFrameCount());
    mStartFrameList.setSlotCount(mEmitter->numTexturePattern());

    const auto freq = mEmitter->texturePatternFrequency();
    const auto mode = freqToAnimMode(freq);

    mTexPatFreq.setValue(freq);
    mAnimModeComboBox.setCurrentEnum(mode);
    mFrameTimeline.setAnimMode(mode);
    mFrameTimeline.setFrameStep(static_cast<s32>(freq));
    mFrameTimeline.setPtclLife(mEmitter->ptclLife());

    if (mode == FrameAnimMode::FitToLife) {
        mTexPatFreq.setDisabled(true);
    } else {
        mTexPatFreq.setDisabled(false);
    }

    mAnimFrameCountSpinBox.setValue(mEmitter->texturePatternFrameCount());

    updateFramesWarning();

    {
        const auto& tbl = mEmitter->texturePatternTable();
        std::vector<s32> frames(tbl.begin(), tbl.end());
        mFrameTimeline.setFrames(frames, mEmitter->texturePatternFrameCount());
    }

    const bool animEnabled = mEmitter->isTexturePatternAnim();
    mTexPatAnimCheckBox.setChecked(animEnabled);
    mTexPatGroupBox.setVisible(animEnabled);
}

void TextureInspector::updateFramesWarning() {
    if (!mEmitter) {
        return;
    }
    const s32 frameCount = mAnimFrameCountSpinBox.value();
    const s32 life = mEmitter->ptclLife();
    const bool showWarning = frameCount > life;
    mFramesWarningLabel->setVisible(showWarning);
}

void TextureInspector::updateDescription() {
    if (!mEmitter || !mEmitter->textureHandle().isValid()) {
        mDescriptionLabel->clear();
        return;
    }

    const s32 divX = mDivXSpinBox.value();
    const s32 divY = mDivYSpinBox.value();
    const s32 repX = mRepXSpinBox.value();
    const s32 repY = mRepYSpinBox.value();

    const f32 tilesPerFrameX = static_cast<f32>(repX) / static_cast<f32>(divX);
    const f32 tilesPerFrameY = static_cast<f32>(repY) / static_cast<f32>(divY);

    mDescriptionLabel->setText(
        QString("%1 frames (%2×%3), each showing %4 × %5 tiles")
            .arg(divX * divY)
            .arg(divX).arg(divY)
            .arg(tilesPerFrameX, 0, 'g', 3)
            .arg(tilesPerFrameY, 0, 'g', 3)
    );
}

void TextureInspector::changeTexture() {
    const auto& textureList = mDocument->textures();

    TextureSelectDialog dialog(textureList, this);
    if (dialog.exec() == QDialog::Accepted) {
        s32 selectedIndex = dialog.selectedIndex();
        if (selectedIndex >= 0 && static_cast<size_t>(selectedIndex) < textureList.size()) {
            const auto texture = textureList.at(selectedIndex).get();
            setEmitterProperty(
                "Set Texture",
                "SetEmitterTexture",
                &Ptcl::Emitter::texture,
                &Ptcl::Emitter::setTexture,
                texture
            );
        }
    }
}

s32 TextureInspector::maxFrameCount() const {
    return mEmitter->numTextureDivisionX() * mEmitter->numTextureDivisionY();
}

std::optional<Math::Vector2f> TextureInspector::calcFrameUVOffset(s32 frame) const {
    const auto divX = mEmitter->numTextureDivisionX();
    const auto divY = mEmitter->numTextureDivisionY();

    if (frame < 0 || divX <= 0 || divY <= 0) {
        return std::nullopt;
    }

    if (frame >= maxFrameCount()) {
        return std::nullopt;
    }

    const s32 frameX = frame % divX;
    const s32 frameY = frame / divX;

    const f32 stepX = 1.0f / static_cast<f32>(divX);
    const f32 stepY = 1.0f / static_cast<f32>(divY);

    return Math::Vector2f{
        stepX * static_cast<f32>(frameX),
        stepY * static_cast<f32>(frameY),
    };
}

QImage TextureInspector::getFrameTexture(s32 frame) const {
    if (!mEmitter->textureHandle().isValid()) {
        return {};
    }

    const s32 divX = mEmitter->numTextureDivisionX();
    const s32 divY = mEmitter->numTextureDivisionY();

    auto uv = calcFrameUVOffset(frame);
    if (!uv) {
        return {};
    }

    const auto& src = mEmitter->textureHandle()->textureData();

    const f32 texW = static_cast<f32>(src.width());
    const f32 texH = static_cast<f32>(src.height());

    const f32 u = uv->getX();
    const f32 v = uv->getY();

    const s32 x = static_cast<s32>(std::round(u * texW));
    const s32 y = static_cast<s32>(std::round(v * texH));

    const s32 w = static_cast<s32>(std::round(texW / static_cast<f32>(divX)));
    const s32 h = static_cast<s32>(std::round(texH / static_cast<f32>(divY)));

    QRect rect(x, y, w, h);
    rect = rect.intersected(src.rect());

    return rect.isValid() ? src.copy(rect) : QImage{};
}

QImage TextureInspector::applyUVRepetition(const QImage& image, f32 repeatX, f32 repeatY) const {
    if (image.isNull()) {
        return {};
    }

    const f32 baseW = static_cast<f32>(image.width());
    const f32 baseH = static_cast<f32>(image.height());

    const s32 outW = static_cast<s32>(std::round(baseW * repeatX));
    const s32 outH = static_cast<s32>(std::round(baseH * repeatY));

    QImage out(outW, outH, image.format());
    out.fill(Qt::transparent);

    if (out.isNull()) {
        return {};
    }

    QPainter p(&out);

    for (s32 y = 0; y < outH; y += image.height()) {
        bool mirrorY = (y / image.height()) % 2 != 0;
        for (s32 x = 0; x < outW; x += image.width()) {
            bool mirrorX = (x / image.width()) % 2 != 0;

            QImage tile = image;
            if (mirrorX) { tile = tile.flipped(Qt::Horizontal); }
            if (mirrorY) { tile = tile.flipped(Qt::Vertical); }
            p.drawImage(x, y, tile);
        }
    }

    return out;
}


// ========================================================================== //


} // namespace PtclEditor
