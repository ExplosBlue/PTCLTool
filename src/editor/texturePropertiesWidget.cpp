#include "editor/texturePropertiesWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QHeaderView>

#include "editor/textureSelectDialog.h"

TexturePropertiesWidget::TexturePropertiesWidget(QWidget* parent) :
    QWidget{parent},
    mEmitterPtr{nullptr},
    mTexPatTbl{1, 16, this},
    mIsPopulating{false} {

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

    // Texture Division X
    connect(&mNumTexPat, &SizedSpinBoxBase::valueChanged, this, &TexturePropertiesWidget::updateNumTexPat);

    // Texture Division X
    connect(&mTexDivX, &SizedSpinBoxBase::valueChanged, this, &TexturePropertiesWidget::updateTexDivX);

    // Texture Division Y
    connect(&mTexDivY, &SizedSpinBoxBase::valueChanged, this, &TexturePropertiesWidget::updateTexDivY);

    // Texture pattern Frequency
    connect(&mTexPatFreq, &SizedSpinBoxBase::valueChanged, this, &TexturePropertiesWidget::updateTexPatFreq);

    // Texture pattern table use
    connect(&mTexPatTblUse, &SizedSpinBoxBase::valueChanged, this, &TexturePropertiesWidget::updateTexPatTblUse);

    // Texture pattern table
    mTexPatTbl.setFixedHeight(60); // small height for one row
    mTexPatTbl.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mTexPatTbl.verticalHeader()->setVisible(false);
    mTexPatTbl.horizontalHeader()->setVisible(false);
    mTexPatTbl.setSelectionMode(QAbstractItemView::NoSelection);
    mTexPatTbl.setEditTriggers(QAbstractItemView::AllEditTriggers);

    // Populate initially
    for (int i = 0; i < 16; ++i) {
        auto* item = new QTableWidgetItem("0");
        item->setTextAlignment(Qt::AlignCenter);
        mTexPatTbl.setItem(0, i, item);
    }

    connect(&mTexPatTbl, &QTableWidget::itemChanged, this, &TexturePropertiesWidget::updateTexPatTbl);

    // UV Scale
    connect(&mUVScaleSpinBox, &VectorSpinBoxBase::valueChanged, this, &TexturePropertiesWidget::updateUVScale);

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
    settingsLayout->addWidget(new QLabel("Texture Division X:"), 3, 0);
    settingsLayout->addWidget(&mTexDivX, 3, 1);
    settingsLayout->addWidget(new QLabel("Texture Division Y:"), 3, 2);
    settingsLayout->addWidget(&mTexDivY, 3, 3);
    settingsLayout->addWidget(new QLabel("Num Texture Pattern:"), 4, 0);
    settingsLayout->addWidget(&mNumTexPat, 4, 1);
    settingsLayout->addWidget(new QLabel("UV Scale:"), 5, 0);
    settingsLayout->addWidget(&mUVScaleSpinBox, 5, 1);
    settingsLayout->addWidget(new QLabel("Texture Pattern Freq:"), 6, 0);
    settingsLayout->addWidget(&mTexPatFreq, 6, 1);
    settingsLayout->addWidget(new QLabel("Texture Pattern Tbl Use:"), 6, 2);
    settingsLayout->addWidget(&mTexPatTblUse, 6, 3);
    settingsLayout->addWidget(new QLabel("Texture Pattern Table:"), 7, 0, 1, 2);
    settingsLayout->addWidget(&mTexPatTbl, 8, 0, 1, 4);


    // Texture Pattern Layout

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

    mIsPopulating = true;

    if (mEmitterPtr->textureHandle().isValid()) {
        mTexturePreview.setPixmap(QPixmap::fromImage(mEmitterPtr->textureHandle()->textureData()));
    }

    mWrapTComboBox.setCurrentEnum(mEmitterPtr->textureWrapT());
    mWrapSComboBox.setCurrentEnum(mEmitterPtr->textureWrapS());
    mMagFilterComboBox.setCurrentEnum(mEmitterPtr->textureMagFilter());
    mMinFilterComboBox.setCurrentEnum(mEmitterPtr->textureMinFilter());
    mMipFilterComboBox.setCurrentEnum(mEmitterPtr->textureMipFilter());

    mNumTexPat.setValue(mEmitterPtr->numTexPat());
    mTexDivX.setValue(mEmitterPtr->numTexDivX());
    mTexDivY.setValue(mEmitterPtr->numTexDivY());
    mTexPatFreq.setValue(mEmitterPtr->texPatFreq());
    mTexPatTblUse.setValue(mEmitterPtr->texPatTblUse());

    const auto& tbl = mEmitterPtr->texPatTbl();
    for (int i = 0; i < tbl.size(); ++i) {
        mTexPatTbl.item(0, i)->setText(QString::number(tbl[i]));
    }

    mUVScaleSpinBox.setVector(mEmitterPtr->texUVScale());

    mIsPopulating = false;
    updateTexPatTblColumns();
}

void TexturePropertiesWidget::setTextureList(const Ptcl::TextureList* textureList) {
    mTextureList = textureList;
}

void TexturePropertiesWidget::updateWrapT() {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    mEmitterPtr->setTextureWrapT(mWrapTComboBox.currentEnum());
}

void TexturePropertiesWidget::updateWrapS() {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    mEmitterPtr->setTextureWrapS(mWrapSComboBox.currentEnum());
}

void TexturePropertiesWidget::updateMinFilter() {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    mEmitterPtr->setTextureMinFilter(mMinFilterComboBox.currentEnum());
}

void TexturePropertiesWidget::updateMagFilter() {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    mEmitterPtr->setTextureMagFilter(mMagFilterComboBox.currentEnum());
}

void TexturePropertiesWidget::updateMipFilter() {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    mEmitterPtr->setTextureMipFilter(mMipFilterComboBox.currentEnum());
}

void TexturePropertiesWidget::updateNumTexPat() {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    mEmitterPtr->setNumTexPat(mNumTexPat.value());
}

void TexturePropertiesWidget::updateTexDivX() {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    mEmitterPtr->setNumTexDivX(mTexDivX.value());
}

void TexturePropertiesWidget::updateTexDivY() {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    mEmitterPtr->setNumTexDivY(mTexDivY.value());
}

void TexturePropertiesWidget::updateUVScale() {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    mEmitterPtr->setTexUVScale(mUVScaleSpinBox.getVector());
}

void TexturePropertiesWidget::updateTexPatFreq() {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    mEmitterPtr->setTexPatFreq(mTexPatFreq.value());
}

void TexturePropertiesWidget::updateTexPatTblUse() {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    mEmitterPtr->setTexPatTblUse(mTexPatTblUse.value());
    updateTexPatTblColumns();
}

void TexturePropertiesWidget::updateTexPatTbl(QTableWidgetItem* item) {
    if (!mEmitterPtr || mIsPopulating) {
        return;
    }

    bool ok = false;
    int value = item->text().toInt(&ok);
    if (!ok || value < 0 || value > 255) {
        item->setText("0");
        return;
    }

    mEmitterPtr->setTexPatTblData(item->column(), value);
}

void TexturePropertiesWidget::updateTextureDetails() {
    mTexturePreview.setPixmap(QPixmap::fromImage(mEmitterPtr->textureHandle()->textureData()));
}

void TexturePropertiesWidget::updateTexPatTblColumns() {
    int use = mEmitterPtr->texPatTblUse();
    const QPalette& palette = mTexPatTbl.palette();

    for (int i = 0; i < 16; ++i) {
        QTableWidgetItem* item = mTexPatTbl.item(0, i);
        if (!item)
            continue;

        if (i >= use) {
            item->setFlags(Qt::NoItemFlags);
            item->setBackground(palette.color(QPalette::Disabled, QPalette::Base));
            item->setForeground(palette.color(QPalette::Disabled, QPalette::Text));
        } else {
            item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
            item->setBackground(palette.color(QPalette::Base));
            item->setForeground(palette.color(QPalette::Text));
        }
    }
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
