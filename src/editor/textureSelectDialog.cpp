#include "editor/textureSelectDialog.h"

#include "util/paintUtil.h"

#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>


// ========================================================================== //


TextureSelectModel::TextureSelectModel(QObject* parent) :
    QAbstractListModel{parent} {}

void TextureSelectModel::setTextures(const Ptcl::TextureList* textures, s32 filterFormat) {
    beginResetModel();

    mTextures = textures;
    rebuildFilter(filterFormat);

    endResetModel();
}

void TextureSelectModel::rebuildFilter(s32 filterFormat) {
    mEntries.clear();

    if (!mTextures) {
        return;
    }

    for (s32 i = 0; i < static_cast<s32>(mTextures->size()); ++i) {
        const auto& tex = (*mTextures)[i];
        if (filterFormat >= 0 && static_cast<s32>(tex->textureFormat()) != filterFormat) {
            continue;
        }
        mEntries.push_back({i});
    }
}


s32 TextureSelectModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid()) {
        return 0;
    }

    return static_cast<s32>(mEntries.size());
}

QVariant TextureSelectModel::data(const QModelIndex& index, s32 role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<s32>(mEntries.size()) || !mTextures) {
        return {};
    }

    const s32 globalIndex = mEntries[index.row()].globalIndex;
    const auto& texture = (*mTextures)[globalIndex];

    switch (role) {
        case Qt::DisplayRole:
            return QStringLiteral("Texture %1")
                .arg(globalIndex);
        case Qt::DecorationRole:
            return QPixmap::fromImage(texture->textureData());
        default:
            return {};
    }
}

s32 TextureSelectModel::globalIndex(s32 row) const {
    if (row < 0 || row >= static_cast<s32>(mEntries.size())) {
        return -1;
    }
    return mEntries[row].globalIndex;
}


// ========================================================================== //


TextureSelectDelegate::TextureSelectDelegate(QObject* parent) :
    QStyledItemDelegate{parent} {}

void TextureSelectDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();

    const bool selected = option.state & QStyle::State_Selected;
    const bool hovered = option.state & QStyle::State_MouseOver;

    if (selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else if (hovered) {
        auto hoverColor = option.palette.highlight().color();
        hoverColor = hoverColor.lighter(120);
        painter->fillRect(option.rect, hoverColor);
    }

    const QRect thumbRect(
        option.rect.left() + (option.rect.width() - sThumbSize) / 2,
        option.rect.top() + sPadding,
        sThumbSize,
        sThumbSize
    );

    PaintUtil::drawCheckerboard(
        *painter,
        thumbRect,
        8,
        thumbRect.size()
    );

    const auto pixmap = index.data(Qt::DecorationRole).value<QPixmap>();

    if (!pixmap.isNull()) {
        const auto scaledPixmap = pixmap.scaled(thumbRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);

        QRect pixmapRect{
            QPoint{},
            scaledPixmap.size()
        };

        pixmapRect.moveCenter(thumbRect.center());
        painter->drawPixmap(pixmapRect.topLeft(), scaledPixmap);
    }

    const QRect textRect(
        option.rect.left() + sPadding,
        thumbRect.bottom() + sPadding,
        option.rect.width() - 2 * sPadding,
        option.rect.height() - thumbRect.height() - 3 * sPadding
    );

    painter->setPen(option.palette.text().color());
    painter->drawText(
        textRect,
        Qt::AlignHCenter | Qt::AlignTop | Qt::TextSingleLine,
        index.data(Qt::DisplayRole).toString()
    );

    painter->restore();
}

QSize TextureSelectDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    Q_UNUSED(index);
    const s32 textHeight = option.fontMetrics.height();

    return {
        sThumbSize + 2 * sPadding,
        3 * sPadding + sThumbSize + textHeight
    };
}


// ========================================================================== //


TextureSelectDialog::TextureSelectDialog(const Ptcl::TextureList& textures, QWidget* parent) :
    QDialog{parent}, mTextures{textures} {

    setWindowTitle("Select Texture");

    mFilterCombo.addItem("All Formats", -1);
    const auto formats = {
        Ptcl::TextureFormat::RGBA8888,
        Ptcl::TextureFormat::RGB888,
        Ptcl::TextureFormat::RGBA5551,
        Ptcl::TextureFormat::RGB565,
        Ptcl::TextureFormat::RGBA4444,
        Ptcl::TextureFormat::LA88,
        Ptcl::TextureFormat::HL8,
        Ptcl::TextureFormat::L8,
        Ptcl::TextureFormat::A8,
        Ptcl::TextureFormat::LA44,
        Ptcl::TextureFormat::L4,
        Ptcl::TextureFormat::A4,
        Ptcl::TextureFormat::ETC1,
        Ptcl::TextureFormat::ETC1_A4,
    };
    for (auto fmt : formats) {
        mFilterCombo.addItem(Ptcl::toString(fmt), static_cast<s32>(fmt));
    }

    auto* filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("Format:"));
    filterLayout->addWidget(&mFilterCombo, 1);

    mListView.setViewMode(QListView::IconMode);
    mListView.setResizeMode(QListView::Adjust);
    mListView.setMovement(QListView::Static);
    mListView.setSpacing(4);
    mListView.setUniformItemSizes(true);
    mListView.setModel(&mModel);
    mListView.setItemDelegate(&mDelegate);

    mModel.setTextures(&mTextures);

    mPreviewWidget.setThumbnailSize({256, 256});
    mPreviewWidget.setMinimumSize(256, 256);

    auto* previewLayout = new QVBoxLayout;
    previewLayout->setContentsMargins(0, 0, 0, 0);
    previewLayout->addWidget(&mPreviewWidget);

    QFont boldFont = mNameLabel.font();
    boldFont.setBold(true);
    mNameLabel.setFont(boldFont);

    previewLayout->addWidget(&mNameLabel);
    previewLayout->addWidget(&mFormatLabel);
    previewLayout->addWidget(&mDimensionsLabel);
    previewLayout->addWidget(&mSizeLabel);
    previewLayout->addWidget(&mUsersLabel);
    previewLayout->addStretch();

    auto* contentLayout = new QHBoxLayout;
    contentLayout->addWidget(&mListView);
    contentLayout->addLayout(previewLayout);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    mOkButton = buttons->button(QDialogButtonBox::Ok);
    mOkButton->setEnabled(false);
    mOkButton->setDefault(true);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(filterLayout);
    mainLayout->addLayout(contentLayout, 1);
    mainLayout->addWidget(buttons);

    connect(&mFilterCombo, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        mModel.setTextures(&mTextures, mFilterCombo.currentData().toInt());
        clearSelection();
    });

    connect(&mListView, &QListView::doubleClicked, this, [this](const QModelIndex& index) {
        if (index.isValid()) {
            mSelectedIndex = mModel.globalIndex(index.row());
            accept();
        }
    });

    connect(mListView.selectionModel(), &QItemSelectionModel::currentChanged, this, &TextureSelectDialog::onSelectionChanged);

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        if (mOkButton->isEnabled() && mSelectedIndex >= 0) {
            accept();
        }
    });

    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

s32 TextureSelectDialog::selectedIndex() const {
    return mSelectedIndex;
}

void TextureSelectDialog::clearSelection() {
    mSelectedIndex = -1;
    mOkButton->setEnabled(false);

    mPreviewWidget.clear();
    mNameLabel.clear();
    mFormatLabel.clear();
    mDimensionsLabel.clear();
    mSizeLabel.clear();
    mUsersLabel.clear();
}

void TextureSelectDialog::onSelectionChanged(const QModelIndex& current, const QModelIndex& previous) {
    Q_UNUSED(previous);

    if (!current.isValid()) {
        return;
    }

    mSelectedIndex = mModel.globalIndex(current.row());
    mOkButton->setEnabled(true);

    const s32 globalIdx = mSelectedIndex;
    const auto& tex = mTextures[globalIdx];
    const auto& img = tex->textureData();
    const auto sizeBytes = tex->textureDataRaw().size();

    mPreviewWidget.setPixmap(QPixmap::fromImage(img));

    mNameLabel.setText(QString("Texture %1").arg(globalIdx));
    mFormatLabel.setText(QString("Format: %1").arg(Ptcl::toString(tex->textureFormat())));
    mDimensionsLabel.setText(QString("Dimensions: %1 x %2").arg(img.width()).arg(img.height()));
    mSizeLabel.setText(sizeBytes < 1024
        ? QString("Size: %1 Bytes").arg(sizeBytes)
        : QString("Size: %1 KB").arg(sizeBytes / 1024));
    mUsersLabel.setText(QString("Users: %1").arg(tex->userCount()));
}


// ========================================================================== //
