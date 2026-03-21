#include "editor/inspector/textureInspector.h"

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


TextureInspector::TextureInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    // Texture Preview
    mTexturePreview.setThumbnailSize(QSize(64, 64));
    mTexDivX.setRange(1, 4);
    mTexDivY.setRange(1, 4);
    mTexPatFreq.setMinimum(1);
    mTexPatTblUse.setRange(2, 16);


    // Texture pattern table
    const s32 referenceHeight = mWrapTComboBox.sizeHint().height(); // Make TexPat table same hight as a comboBox
    mTexPatTbl.setFixedHeight(referenceHeight);
    mTexPatTbl.setRowCount(1);
    mTexPatTbl.setRowHeight(0, referenceHeight);
    mTexPatTbl.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mTexPatTbl.verticalHeader()->setVisible(false);
    mTexPatTbl.horizontalHeader()->setVisible(false);
    mTexPatTbl.setSelectionMode(QAbstractItemView::NoSelection);
    mTexPatTbl.setEditTriggers(QAbstractItemView::AllEditTriggers);

    // Populate initially
    for (s32 i = 0; i < 16; ++i) {
        auto* item = new QTableWidgetItem("0");
        item->setTextAlignment(Qt::AlignCenter);
        mTexPatTbl.setItem(0, i, item);
    }

    mTexRepetitionsX.setMinimum(1);
    mTexRepetitionsY.setMinimum(1);

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
    settingsLayout->addWidget(new QLabel("Mipmap Filter:"), 2, 2);
    settingsLayout->addWidget(&mMipFilterComboBox, 2, 3);
    settingsLayout->addWidget(new QLabel("Texture Columns:"), 3, 0);
    settingsLayout->addWidget(&mTexDivX, 3, 1);
    settingsLayout->addWidget(new QLabel("Texture Rows:"), 3, 2);
    settingsLayout->addWidget(&mTexDivY, 3, 3);
    settingsLayout->addWidget(new QLabel("Repetitions X:"), 4, 0);
    settingsLayout->addWidget(&mTexRepetitionsX, 4, 1);
    settingsLayout->addWidget(new QLabel("Repetitions Y:"), 4, 2);
    settingsLayout->addWidget(&mTexRepetitionsY, 4, 3);

    // Anim Mode
    mAnimModeComboBox.addItems({"Fit to Speed", "Fit to Life"});

    // Texture Pattern Anim
    mTexPatGroupBox.setTitle("Texture Pattern Animation");
    mTexPatGroupBox.setCheckable(true);

    auto texPatSettingsLayout = new QGridLayout;
    texPatSettingsLayout->addWidget(new QLabel("Animation Mode:"), 0, 0);
    texPatSettingsLayout->addWidget(&mAnimModeComboBox, 0, 1);
    texPatSettingsLayout->addWidget(new QLabel("Frames Per Step:"), 1, 0);
    texPatSettingsLayout->addWidget(&mTexPatFreq, 1, 1);
    texPatSettingsLayout->addWidget(new QLabel("Animation Frames:"), 1, 2);
    texPatSettingsLayout->addWidget(&mTexPatTblUse, 1, 3);
    texPatSettingsLayout->addWidget(new QLabel("Frame Order:"), 2, 0, 1, 2);
    texPatSettingsLayout->addWidget(&mTexPatTbl, 2, 0, 1, 4);

    mTexPatGroupBox.setLayout(texPatSettingsLayout);

    // Main Layout
    auto mainLayout = new QGridLayout(this);
    mainLayout->addLayout(settingsLayout, 0, 0, 1, 1);
    mainLayout->addWidget(&mTexturePreview, 0, 1, 1, 1);
    mainLayout->addWidget(&mTexPatGroupBox, 2, 0, 1, 2);

    mainLayout->addWidget(new QLabel("Initial Frame Variants:"), 3, 0, 1, 1);
    mainLayout->addWidget(&mNumTexPat, 3, 1, 1, 1);

    setLayout(mainLayout);
    setupConnections();
}

void TextureInspector::setupConnections() {
    connect(&mTexturePreview, &ThumbnailWidget::clicked, this, &TextureInspector::changeTexture);

    // Wrap T
    connect(&mWrapTComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto wrap = mWrapTComboBox.currentEnum();
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
        setEmitterProperty(
            "Set Texture Wrap V",
            "SetTexWrapV",
            &Ptcl::Emitter::textureWrapS,
            &Ptcl::Emitter::setTextureWrapS,
            wrap
        );
    });

    // Mag Filter
    connect(&mMagFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto filter = mMagFilterComboBox.currentEnum();
        setEmitterProperty(
            "Set Texture Mag Filter",
            "SetTexMagFilter",
            &Ptcl::Emitter::textureMagFilter,
            &Ptcl::Emitter::setTextureMagFilter,
            filter
        );
    });

    // Min Filter
    connect(&mMinFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto filter = mMinFilterComboBox.currentEnum();
        setEmitterProperty(
            "Set Texture Min Filter",
            "SetTexMinFilter",
            &Ptcl::Emitter::textureMinFilter,
            &Ptcl::Emitter::setTextureMinFilter,
            filter
        );
    });

    // Mipmap Filter
    connect(&mMipFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto filter = mMipFilterComboBox.currentEnum();
        setEmitterProperty(
            "Set Texture Mipmap Filter",
            "SetTexMipFilter",
            &Ptcl::Emitter::textureMipFilter,
            &Ptcl::Emitter::setTextureMipFilter,
            filter
        );
    });

    // Texture pattern count
    connect(&mNumTexPat, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture Pattern Count",
            "SetNumTexPat",
            &Ptcl::Emitter::numTexturePattern,
            &Ptcl::Emitter::setNumTexturePattern,
            static_cast<u16>(value)
        );
    });

    connect(&mTexDivX, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture Divisions X",
            "SetTexDivX",
            &Ptcl::Emitter::numTextureDivisionX,
            &Ptcl::Emitter::setNumTextureDivisionX,
            static_cast<u8>(value)
        );
    });

    connect(&mTexDivY, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture Divisions Y",
            "SetTexDivY",
            &Ptcl::Emitter::numTextureDivisionY,
            &Ptcl::Emitter::setNumTextureDivisionY,
            static_cast<u8>(value)
        );
    });

    connect(&mTexPatFreq, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture Anim Frame Step",
            "SetTexPatFreq",
            &Ptcl::Emitter::texturePatternFrequency,
            &Ptcl::Emitter::setTexturePatternFrequency,
            static_cast<u16>(value)
        );
    });

    connect(&mTexPatTblUse, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture Anim Frame Count",
            "SetTexPatTblUse",
            &Ptcl::Emitter::texturePatternTableUse,
            &Ptcl::Emitter::setTexturePatternTableUse,
            static_cast<u16>(value)
        );
    });

    connect(&mTexPatTbl, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
        QSignalBlocker b1(mTexPatTbl);

        bool ok = false;
        s32 value = item->text().toInt(&ok);

        const bool valid = ok && value >= 0 && value < maxFrameCount();
        if (!valid) {
            value = 0;
            item->setText("0");
        }

        auto table = mEmitter->texturePatternTable();
        const s32 col = item->column();

        if (table[col] == value) {
            return;
        }

        table[col] = value;

        setEmitterProperty(
            "Set Texture Anim Frame Order",
            QString("SetTexPatTbl_%1").arg(col),
            &Ptcl::Emitter::texturePatternTable,
            &Ptcl::Emitter::setTexturePatternTable,
            table
        );
    });

    connect(&mTexRepetitionsX, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture X Repetition",
            "SetTexRepX",
            &Ptcl::Emitter::numTextureRepetitionsX,
            &Ptcl::Emitter::setNumTextureRepetitionsX,
            static_cast<s32>(value)
        );
    });

    connect(&mTexRepetitionsY, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture Y Repetition",
            "SetTexRepY",
            &Ptcl::Emitter::numTextureRepetitionsY,
            &Ptcl::Emitter::setNumTextureRepetitionsY,
            static_cast<s32>(value)
        );
    });

    connect(&mAnimModeComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        auto mode = static_cast<AnimMode>(index);

        u16 freq;
        if (mode == AnimMode::FitToLife) {
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

    connect(&mTexPatGroupBox, &QGroupBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Texture Anim",
            "ToggleTexAnim",
            &Ptcl::Emitter::isTexturePatternAnim,
            &Ptcl::Emitter::setIsTexturePatternAnim,
            checked
        );
    });
}

void TextureInspector::populateProperties() {
    QSignalBlocker b1(mWrapTComboBox);
    QSignalBlocker b2(mWrapSComboBox);
    QSignalBlocker b3(mMagFilterComboBox);
    QSignalBlocker b4(mMinFilterComboBox);
    QSignalBlocker b5(mMipFilterComboBox);
    QSignalBlocker b6(mNumTexPat);
    QSignalBlocker b7(mTexDivX);
    QSignalBlocker b8(mTexDivY);
    QSignalBlocker b9(mTexPatFreq);
    QSignalBlocker b10(mTexPatTblUse);
    QSignalBlocker b11(mTexPatTbl);
    QSignalBlocker b12(mTexPatGroupBox);
    QSignalBlocker b13(mTexRepetitionsX);
    QSignalBlocker b14(mTexRepetitionsY);
    QSignalBlocker b15(mAnimModeComboBox);

    if (mEmitter->textureHandle().isValid()) {
        mTexturePreview.setPixmap(QPixmap::fromImage(mEmitter->textureHandle()->textureData()));
    }

    mWrapTComboBox.setCurrentEnum(mEmitter->textureWrapT());
    mWrapSComboBox.setCurrentEnum(mEmitter->textureWrapS());
    mMagFilterComboBox.setCurrentEnum(mEmitter->textureMagFilter());
    mMinFilterComboBox.setCurrentEnum(mEmitter->textureMinFilter());
    mMipFilterComboBox.setCurrentEnum(mEmitter->textureMipFilter());

    mNumTexPat.setValue(mEmitter->numTexturePattern());
    mTexDivX.setValue(mEmitter->numTextureDivisionX());
    mTexDivY.setValue(mEmitter->numTextureDivisionY());

    const auto freq = mEmitter->texturePatternFrequency();
    const auto mode = freqToAnimMode(freq);

    mTexPatFreq.setValue(freq);
    mAnimModeComboBox.setCurrentIndex(static_cast<s32>(mode));

    if (mode == AnimMode::FitToLife) {
        mTexPatFreq.setDisabled(true);
    } else {
        mTexPatFreq.setDisabled(false);
    }

    mTexPatTblUse.setValue(mEmitter->texturePatternTableUse());

    const auto& tbl = mEmitter->texturePatternTable();
    for (s32 i = 0; i < tbl.size(); ++i) {
        QTableWidgetItem* item = mTexPatTbl.item(0, i);
        item->setText(QString::number(tbl[i]));

        if (i < mEmitter->numTexturePattern()) {
            item->setIcon(QIcon(createFramePreview(tbl[i])));
        }
    }

    mTexPatGroupBox.setChecked(mEmitter->isTexturePatternAnim());

    mTexRepetitionsX.setValue(mEmitter->numTextureRepetitionsX());
    mTexRepetitionsY.setValue(mEmitter->numTextureRepetitionsY());

    updateTexPatTblColumns();
}

void TextureInspector::updateTexPatTblColumns() {
    const QPalette& palette = mTexPatTbl.palette();

    for (s32 i = 0; i < 16; ++i) {
        QTableWidgetItem* item = mTexPatTbl.item(0, i);
        if (!item) {
            continue;
        }

        if (i >= mEmitter->texturePatternTableUse()) {
            item->setFlags(Qt::NoItemFlags);
            item->setBackground(palette.color(QPalette::Disabled, QPalette::Base));
            item->setForeground(palette.color(QPalette::Disabled, QPalette::Text));

            s32 frame = mEmitter->texturePatternTable()[i];
            item->setText(QString::number(frame));
            item->setIcon(QIcon());
        } else {
            item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
            item->setBackground(palette.color(QPalette::Base));
            item->setForeground(palette.color(QPalette::Text));

            s32 frame = mEmitter->texturePatternTable()[i];
            item->setText(QString::number(frame));
            item->setIcon(QIcon(createFramePreview(frame)));
        }
    }
}

void TextureInspector::changeTexture() {
    const auto& textureList = mDocument->textures();

    TextureSelectDialog dialog(textureList, this);
    if (dialog.exec() == QDialog::Accepted) {
        s32 selectedInded = dialog.selectedIndex();
        if (selectedInded >= 0 && static_cast<size_t>(selectedInded) < textureList.size()) {
            const auto texture = textureList.at(selectedInded).get();
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

    const auto& uvScale = mEmitter->textureUVScale();

    const s32 frameX = frame % divX;
    const s32 frameY = frame / divX;

    const Math::Vector2f uvOffset{
        uvScale.getX() * static_cast<f32>(frameX),
        uvScale.getY() * static_cast<f32>(frameY),
    };

    return uvOffset;
}

QImage TextureInspector::getFrameTexture(s32 frame) const {
    if (!mEmitter->textureHandle().isValid()) {
        return {};
    }

    auto uv = calcFrameUVOffset(frame);
    if (!uv) {
        return {};
    }

    const auto& uvOffset = *uv;
    const auto& uvScale = mEmitter->textureUVScale();
    const auto& src = mEmitter->textureHandle()->textureData();

    const f32 texW = static_cast<f32>(src.width());
    const f32 texH = static_cast<f32>(src.height());

    const s32 x = static_cast<s32>(std::round(uvOffset.getX() * texW));
    const s32 y = static_cast<s32>(std::round(uvOffset.getY() * texH));
    const s32 w = static_cast<s32>(std::round(uvScale.getX() * texW));
    const s32 h = static_cast<s32>(std::round(uvScale.getY() * texH));

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

QPixmap TextureInspector::createFramePreview(s32 frame) const {
    QImage base = getFrameTexture(frame);
    if (base.isNull()) {
        return {};
    }

    // TODO
    const f32 repeatX = static_cast<f32>(mTexRepetitionsX.value());
    const f32 repeatY = static_cast<f32>(mTexRepetitionsY.value());

    QImage repeated = applyUVRepetition(base, repeatX, repeatY);
    if (repeated.isNull()) {
        return {};
    }

    return QPixmap::fromImage(repeated.scaled(32, 32, Qt::KeepAspectRatio));
}


// ========================================================================== //


} // namespace PtclEditor
