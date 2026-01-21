#include "editor/texturePropertiesWidget.h"
#include "editor/textureSelectDialog.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>


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

    // Texture Division X
    connect(&mNumTexPat, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mProps.numTexPat = value;
        emit propertiesUpdated(mProps);
    });

    // Texture Division X
    connect(&mTexDivX, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mProps.numTexDivX = value;
        emit propertiesUpdated(mProps);
    });

    // Texture Division Y
    connect(&mTexDivY, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mProps.numTexDivY = value;
        emit propertiesUpdated(mProps);
    });

    // Texture pattern Frequency
    connect(&mTexPatFreq, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mProps.texPatFreq = value;
        emit propertiesUpdated(mProps);
    });

    // Texture pattern table use
    mTexPatTblUse.setRange(0, 16);
    connect(&mTexPatTblUse, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mProps.texPatTblUse = value;
        updateTexPatTblColumns();
        emit propertiesUpdated(mProps);
    });

    // Texture pattern table
    mTexPatTbl.setFixedHeight(60);
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

    connect(&mTexPatTbl, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
        bool ok = false;
        s32 value = item->text().toInt(&ok);
        if (!ok || value < 0 || value > 255) {
            item->setText("0");
            return;
        }

        mProps.texPatTbl[item->column()] = value;
        emit propertiesUpdated(mProps);
    });

    // UV Scale
    connect(&mUVScaleSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mProps.texUVScale = mUVScaleSpinBox.getVector();
        emit propertiesUpdated(mProps);
    });

    // TexPatAnim
    connect(&mTexPatAnimCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mProps.isTexPatAnim = checked;
        emit propertiesUpdated(mProps);
    });

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
    settingsLayout->addWidget(new QLabel("Texture Pattern Anim Enabled:"), 7, 0);
    settingsLayout->addWidget(&mTexPatAnimCheckBox, 7, 1);
    settingsLayout->addWidget(new QLabel("Texture Pattern Table:"), 8, 0, 1, 2);
    settingsLayout->addWidget(&mTexPatTbl, 9, 0, 1, 4);

    // Main Layout
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(&mTexturePreview);
    mainLayout->addLayout(settingsLayout);
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
    QSignalBlocker b12(mUVScaleSpinBox);
    QSignalBlocker b13(mTexPatAnimCheckBox);

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
    mTexPatFreq.setValue(mProps.texPatFreq);
    mTexPatTblUse.setValue(mProps.texPatTblUse);

    const auto& tbl = mProps.texPatTbl;
    for (s32 i = 0; i < tbl.size(); ++i) {
        mTexPatTbl.item(0, i)->setText(QString::number(tbl[i]));
    }

    mUVScaleSpinBox.setVector(mProps.texUVScale);
    mTexPatAnimCheckBox.setChecked(mProps.isTexPatAnim);

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
        } else {
            item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
            item->setBackground(palette.color(QPalette::Base));
            item->setForeground(palette.color(QPalette::Text));
        }
    }
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
}


// ========================================================================== //
