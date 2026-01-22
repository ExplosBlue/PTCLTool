#include "editor/texturePropertiesWidget.h"
#include "editor/textureSelectDialog.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QGroupBox>


// ========================================================================== //


TexturePropertiesWidget::TexturePropertiesWidget(QWidget* parent) :
    QWidget{parent} {
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

    // Texture Preview
    mTexturePreview.setThumbnailSize(QSize(64, 64));
    connect(&mTexturePreview, &ThumbnailWidget::clicked, this, &TexturePropertiesWidget::changeTexture);

    // Texture pattern count
    connect(&mNumTexPat, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mProps.numTexPat = value;
        emit propertiesUpdated(mProps);
    });

    // Texture Division X
    mTexDivX.setRange(1, 4);
    connect(&mTexDivX, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mProps.numTexDivX = value;
        updateUVScale();
        updateTexPatTblColumns();
        emit propertiesUpdated(mProps);
    });

    // Texture Division Y
    mTexDivY.setRange(1, 4);
    connect(&mTexDivY, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mProps.numTexDivY = value;
        updateUVScale();
        updateTexPatTblColumns();
        emit propertiesUpdated(mProps);
    });

    // Texture pattern Frequency
    mTexPatFreq.setMinimum(1);
    connect(&mTexPatFreq, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mProps.texPatFreq = value;
        emit propertiesUpdated(mProps);
    });

    // Texture pattern table use
    mTexPatTblUse.setRange(2, 16);
    connect(&mTexPatTblUse, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mProps.texPatTblUse = value;
        updateTexPatTblColumns();
        emit propertiesUpdated(mProps);
    });

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

        if (i < mProps.numTexPat) {
            item->setIcon(QIcon(createFramePreview(mProps.texPatTbl[i])));
        }

        mTexPatTbl.setItem(0, i, item);
    }

    connect(&mTexPatTbl, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
        QSignalBlocker b1(mTexPatTbl);

        bool ok = false;
        s32 value = item->text().toInt(&ok);

        const bool valid = ok && value >= 0 && value < maxFrameCount();

        if (!valid) {
            value = 0;
            item->setText("0");
        }

        mProps.texPatTbl[item->column()] = value;
        item->setIcon(valid ? QIcon(createFramePreview(value)) : QIcon());

        emit propertiesUpdated(mProps);
    });

    // Texture Repetitions X
    mTexRepetitionsX.setMinimum(1);
    connect(&mTexRepetitionsX, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        f32 uvX = static_cast<f32>(value) / static_cast<f32>(mProps.numTexDivX);
        mProps.texUVScale.setX(uvX);
        updateTexPatTblColumns();
        emit propertiesUpdated(mProps);
    });

    // Texture Repetitions Y
    mTexRepetitionsY.setMinimum(1);
    connect(&mTexRepetitionsY, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        f32 uvY = static_cast<f32>(value) / static_cast<f32>(mProps.numTexDivY);
        mProps.texUVScale.setY(uvY);
        updateTexPatTblColumns();
        emit propertiesUpdated(mProps);
    });


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
    connect(&mAnimModeComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        auto mode = mAnimModeComboBox.currentEnum();

        if (mode == AnimMode::FitToLife) {
            mTexPatFreq.setDisabled(true);
            mProps.texPatFreq = 0;
        } else {
            mTexPatFreq.setDisabled(false);
            mProps.texPatFreq = 1;
        }

        emit propertiesUpdated(mProps);
    });

    // Texture Pattern Anim
    mTexPatGroupBox.setTitle("Texture Pattern Animation");
    mTexPatGroupBox.setCheckable(true);
    connect(&mTexPatGroupBox, &QGroupBox::clicked, this, [this](bool checked) {
        mProps.isTexPatAnim = checked;
        emit propertiesUpdated(mProps);
    });

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

}

void TexturePropertiesWidget::setProperties(const Ptcl::TextureProperties& properties, const std::shared_ptr<Ptcl::Texture>& texture) {
    mProps = properties;
    mTexture = texture;
    populateWidgets();
}

void TexturePropertiesWidget::populateWidgets() {
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

    if (mTexture) {
        mTexturePreview.setPixmap(QPixmap::fromImage(mTexture->textureData()));
    }

    mWrapTComboBox.setCurrentEnum(mProps.textureWrapT);
    mWrapSComboBox.setCurrentEnum(mProps.textureWrapS);
    mMagFilterComboBox.setCurrentEnum(mProps.textureMagFilter);
    mMinFilterComboBox.setCurrentEnum(mProps.textureMinFilter);
    mMipFilterComboBox.setCurrentEnum(mProps.textureMipFilter);

    mNumTexPat.setValue(mProps.numTexPat);
    mTexDivX.setValue(mProps.numTexDivX);
    mTexDivY.setValue(mProps.numTexDivY);

    const auto freq = mProps.texPatFreq;
    const auto mode = freqToAnimMode(freq);

    mTexPatFreq.setValue(mProps.texPatFreq);
    mAnimModeComboBox.setCurrentEnum(mode);

    if (mode == AnimMode::FitToLife) {
        mTexPatFreq.setDisabled(true);
    } else {
        mTexPatFreq.setDisabled(false);
    }

    mTexPatTblUse.setValue(mProps.texPatTblUse);

    const auto& tbl = mProps.texPatTbl;
    for (s32 i = 0; i < tbl.size(); ++i) {
        QTableWidgetItem* item = mTexPatTbl.item(0, i);
        item->setText(QString::number(tbl[i]));

        if (i < mProps.numTexPat) {
            item->setIcon(QIcon(createFramePreview(mProps.texPatTbl[i])));
        }
    }

    mTexPatGroupBox.setChecked(mProps.isTexPatAnim);

    const f32 divX = static_cast<f32>(mProps.numTexDivX);
    const f32 divY = static_cast<f32>(mProps.numTexDivY);

    mTexRepetitionsX.setValue(static_cast<s32>(std::round(mProps.texUVScale.getX() * divX)));
    mTexRepetitionsY.setValue(static_cast<s32>(std::round(mProps.texUVScale.getY() * divY)));

    updateTexPatTblColumns();
}

void TexturePropertiesWidget::setTextureList(const Ptcl::TextureList* textureList) {
    mTextureList = textureList;
}

void TexturePropertiesWidget::updateTextureDetails() {
    mTexturePreview.setPixmap(QPixmap::fromImage(mTexture->textureData()));
}

void TexturePropertiesWidget::updateTexPatTblColumns() {
    const QPalette& palette = mTexPatTbl.palette();

    for (s32 i = 0; i < 16; ++i) {
        QTableWidgetItem* item = mTexPatTbl.item(0, i);
        if (!item) {
            continue;
        }

        if (i >= mProps.texPatTblUse) {
            item->setFlags(Qt::NoItemFlags);
            item->setBackground(palette.color(QPalette::Disabled, QPalette::Base));
            item->setForeground(palette.color(QPalette::Disabled, QPalette::Text));

            s32 frame = mProps.texPatTbl[i];
            item->setText(QString::number(frame));
            item->setIcon(QIcon());
        } else {
            item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
            item->setBackground(palette.color(QPalette::Base));
            item->setForeground(palette.color(QPalette::Text));

            s32 frame = mProps.texPatTbl[i];
            item->setText(QString::number(frame));
            item->setIcon(QIcon(createFramePreview(frame)));
        }
    }
}

void TexturePropertiesWidget::updateUVScale() {
    mProps.texUVScale.setX(static_cast<f32>(mTexRepetitionsX.value()) / static_cast<f32>(mProps.numTexDivX));
    mProps.texUVScale.setY(static_cast<f32>(mTexRepetitionsY.value()) / static_cast<f32>(mProps.numTexDivY));

}

void TexturePropertiesWidget::changeTexture() {
    if (!mTextureList) {
        return;
    }

    auto oldTexture = mTexture;

    TextureSelectDialog dialog(*mTextureList, this);
    if (dialog.exec() == QDialog::Accepted) {
        s32 selectedInded = dialog.selectedIndex();
        if (selectedInded >= 0 && static_cast<size_t>(selectedInded) < mTextureList->size()) {
            mTexture = mTextureList->at(selectedInded);
            updateTextureDetails();
            emit textureUpdated(oldTexture, mTexture);
        }
    }

    updateTexPatTblColumns();
}

s32 TexturePropertiesWidget::maxFrameCount() const {
    return mProps.numTexDivX * mProps.numTexDivY;
}

std::optional<Math::Vector2f> TexturePropertiesWidget::calcFrameUVOffset(s32 frame) const {
    const auto divX = mProps.numTexDivX;
    const auto divY = mProps.numTexDivY;

    if (frame < 0 || divX <= 0 || divY <= 0) {
        return std::nullopt;
    }

    if (frame >= maxFrameCount()) {
        return std::nullopt;
    }

    const auto& uvScale = mProps.texUVScale;

    const s32 frameX = frame % divX;
    const s32 frameY = frame / divX;

    const Math::Vector2f uvOffset{
        uvScale.getX() * static_cast<f32>(frameX),
        uvScale.getY() * static_cast<f32>(frameY),
    };

    return uvOffset;
}

QImage TexturePropertiesWidget::getFrameTexture(s32 frame) const {
    auto uv = calcFrameUVOffset(frame);
    if (!uv) {
        return {};
    }

    const auto& uvOffset = *uv;
    const auto& uvScale = mProps.texUVScale;
    const auto& src = mTexture->textureData();

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

QImage TexturePropertiesWidget::applyUVRepetition(const QImage& image, f32 repeatX, f32 repeatY) const {
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

QPixmap TexturePropertiesWidget::createFramePreview(s32 frame) const {
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
