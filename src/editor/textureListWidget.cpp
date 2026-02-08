#include "editor/textureListWidget.h"
#include "editor/textureImportDialog.h"

#include "ptcl/ptcl.h"
#include "util/paintUtil.h"
#include "util/settingsUtil.h"

#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMenu>
#include <QPainter>
#include <QSplitter>
#include <QStandardItemModel>
#include <QStandardPaths>


namespace PtclEditor {


// ========================================================================== //


TextureItemDelegate::TextureItemDelegate(QObject* parent) :
    QStyledItemDelegate{parent} {}

void TextureItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    const QRect thumbRect(
        option.rect.left() + sPaddingH,
        option.rect.top() + (option.rect.height() - sThumbSize) / 2,
        sThumbSize,
        sThumbSize
    );

    PaintUtil::drawCheckerboard(*painter, thumbRect);

    const auto pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
    if (!pixmap.isNull()) {
        const auto scaledPixmap = pixmap.scaled(thumbRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QRect centeredRect = QRect(thumbRect.topLeft(), scaledPixmap.size());
        centeredRect.moveCenter(thumbRect.center());
        painter->drawPixmap(centeredRect.topLeft(), scaledPixmap);
    }

    const QRect textRect(
        thumbRect.right() + sPaddingH,
        option.rect.top(),
        option.rect.width() - thumbRect.width() - (2 * sPaddingH),
        option.rect.height()
    );

    const auto text = index.data(Qt::DisplayRole).toString();
    painter->setPen(option.palette.text().color());
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, text);

    painter->restore();
}

QSize TextureItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    const QString text = index.data(Qt::DisplayRole).toString();
    QFontMetrics fm(option.font);

    QRect textRect = fm.boundingRect(QRect(0, 0, 1000, 1000), Qt::TextWordWrap, text);
    s32 width = sPaddingH + sThumbSize + sPaddingH + textRect.width() + sPaddingH;
    s32 height = std::max(sThumbSize, textRect.height()) + (2 * sPaddingV);

    return {width, height};
}


// ========================================================================== //


TextureListModel::TextureListModel(QObject* parent) :
    QAbstractListModel{parent} {}

void TextureListModel::setTextures(Ptcl::TextureList* textures) {
    beginResetModel();

    mTextures = textures;

    if (mTextures) {
        for (auto & tex : *mTextures) {
            tex->setUserCountCallback([this, texture = tex.get()]() {
                emitRowChangedFor(texture);
            });
        }
    }

    endResetModel();
}

s32 TextureListModel::rowCount(const QModelIndex& index) const {
    Q_UNUSED(index);
    return mTextures ? static_cast<s32>(mTextures->size()) : 0;
}

QVariant TextureListModel::data(const QModelIndex& index, s32 role) const {
    if (!mTextures || !index.isValid()) {
        return {};
    }

    const auto& texture = (*mTextures)[index.row()];

    switch (role) {
        case Qt::DisplayRole: {
            const auto& img = texture->textureData();
            const auto sizeBytes = texture->textureDataRaw().size();

            const QString sizeString = (sizeBytes < 1024)
                ? QString("%1 Bytes").arg(sizeBytes)
                : QString("%1 KB").arg(sizeBytes / 1024);

            return QString("Format: %1\nDimensions: %2x%3\nSize %4\nUsers: %5")
                .arg(toString(texture->textureFormat()))
                .arg(img.width())
                .arg(img.height())
                .arg(sizeString)
                .arg(texture->userCount());
        }
        case Qt::DecorationRole:
            return QPixmap::fromImage(texture->textureData());
        case Roles::TexturePtrRole:
            return QVariant::fromValue<void*>(texture.get());
        case Roles::UserCountRole:
            return texture->userCount();
        default:
            return {};
    }
}

void TextureListModel::emitRowChangedFor(Ptcl::Texture* texture) {
    if (!mTextures || !texture) {
        return;
    }

    const s32 rowCount = static_cast<s32>(mTextures->size());
    for (s32 row = 0; row < rowCount; ++row) {
        if ((*mTextures)[row].get() == texture) {
            QModelIndex idx = index(row);
            emit dataChanged(idx, idx, {Qt::DisplayRole, UserCountRole});
            return;
        }
    }
}


// ========================================================================== //

TextureDetailsPanel::TextureDetailsPanel(QWidget* parent) :
    QWidget{parent} {
    mThumbnailWidget.setThumbnailSize({256, 256});
    mExportButton.setText("Export Texture");
    mReplaceButton.setText("Replace Texture");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addWidget(&mThumbnailWidget);

    auto* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(&mExportButton);
    buttonLayout->addWidget(&mReplaceButton);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch(1);

    connect(&mExportButton, &QPushButton::clicked, this, [this](bool checked) {
        Q_UNUSED(checked);
        if (mTexturePtr) {
            emit exportRequested(mTexturePtr);
        }
    });

    connect(&mReplaceButton, &QPushButton::clicked, this, [this](bool checked) {
        Q_UNUSED(checked);
        if (mTexturePtr) {
            emit replaceRequested(mTexturePtr);
        }
    });

    setEnabled(false);
}

void TextureDetailsPanel::setTexture(Ptcl::Texture* texture) {
    mTexturePtr = texture;

    if (!mTexturePtr) {
        setEnabled(false);
        mThumbnailWidget.clear();
        return;
    }

    setEnabled(true);
    mThumbnailWidget.setPixmap(QPixmap::fromImage(mTexturePtr->textureData()));
}


// ========================================================================== //


TextureListWidget::TextureListWidget(QWidget *parent) :
    QWidget{parent} {
    setupToolbar();
    setupView();
    setupContextMenu();
    setupLayout();
    setupSelectionHandling();

    connect(&mDetailsPanel, &TextureDetailsPanel::exportRequested, this, &TextureListWidget::exportTexture);
    connect(&mDetailsPanel, &TextureDetailsPanel::replaceRequested, this, &TextureListWidget::replaceTexture);
}

void TextureListWidget::setupToolbar() {
    mToolbar.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mActionExportAll = mToolbar.addAction(QIcon(":/res/icons/export_image.png"), "Export All");
    mActionImportTexture = mToolbar.addAction(QIcon(":/res/icons/import_image.png"), "Import Texture");

    connect(mActionExportAll, &QAction::triggered, this, &TextureListWidget::exportAll);
    connect(mActionImportTexture, &QAction::triggered, this, &TextureListWidget::importTexture);
}

void TextureListWidget::setupView() {
    mView.setViewMode(QListView::IconMode);
    mView.setResizeMode(QListView::Adjust);
    mView.setSpacing(10);
    mView.setUniformItemSizes(true);
    mView.setMouseTracking(true);
    mView.setSelectionMode(QAbstractItemView::SingleSelection);
    mView.setContextMenuPolicy(Qt::CustomContextMenu);
    mView.setModel(&mModel);
    mView.setItemDelegate(&mDelegate);
}

void TextureListWidget::setupContextMenu() {
    connect(&mView, &QListView::customContextMenuRequested, this, [this](const QPoint& pos) {
        QModelIndex index = mView.indexAt(pos);
        if (!index.isValid()) {
            return;
        }

        auto* texture = static_cast<Ptcl::Texture*>(index.data(TextureListModel::Roles::TexturePtrRole).value<void*>());

        if (!texture) {
            return;
        }

        QMenu menu(this);
        menu.addAction("Export", this, [this, texture] {
            exportTexture(texture);
        });
        menu.addAction("Replace", this, [this, texture] {
            replaceTexture(texture);
        });

        menu.exec(mView.viewport()->mapToGlobal(pos));
    });
}

void TextureListWidget::setupLayout() {
    auto* mainLayout = new QVBoxLayout(this);

    auto* listLayout = new QHBoxLayout;
    listLayout->addWidget(&mView);
    listLayout->addWidget(&mDetailsPanel);
    mainLayout->addLayout(listLayout);

    mainLayout->addWidget(&mToolbar);
}

void TextureListWidget::setupSelectionHandling() {
    connect(mView.selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex& current, const QModelIndex& previous) {
        Q_UNUSED(previous);

        Ptcl::Texture* texture = nullptr;
        if (current.isValid()) {
            texture = static_cast<Ptcl::Texture*>(current.data(TextureListModel::Roles::TexturePtrRole).value<void*>());
        }
        mDetailsPanel.setTexture(texture);
    });
}

void TextureListWidget::setTextures(Ptcl::TextureList* textures) {
    if (mTexturesPtr == textures) {
        return;
    }

    mTexturesPtr = textures;
    mModel.setTextures(textures);

    const bool hasData = (textures != nullptr);
    mActionExportAll->setEnabled(hasData);
    mActionImportTexture->setEnabled(hasData);
}

void TextureListWidget::clear() {
    setTextures(nullptr);
    mDetailsPanel.setTexture(nullptr);
}

void TextureListWidget::exportAll() {
    if (!mTexturesPtr) {
        return;
    }

    QString basePath = SettingsUtil::SettingsMgr::instance().lastExportPath();
    if (basePath.isEmpty()) {
        QString lastOpenPath = SettingsUtil::SettingsMgr::instance().lastOpenPath();
        if (!lastOpenPath.isEmpty()) {
            basePath = lastOpenPath;
        } else {
            basePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
    }

    QString dirPath = QFileDialog::getExistingDirectory(this, "Export textures", basePath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dirPath.isEmpty()) {
        return;
    }

    for (s32 idx = 0; idx < mTexturesPtr->size(); ++idx) {
        const auto& texture = (*mTexturesPtr)[idx];
        texture->textureData().save(QString("%1/tex_%2.png").arg(dirPath).arg(idx));
    }

    SettingsUtil::SettingsMgr::instance().setLastExportPath(QFileInfo(dirPath).absolutePath());
}

void TextureListWidget::importTexture() {
    if (!mTexturesPtr) {
        return;
    }

    QString basePath = SettingsUtil::SettingsMgr::instance().lastImportPath();
    if (basePath.isEmpty()) {
        QString lastOpenPath = SettingsUtil::SettingsMgr::instance().lastOpenPath();
        if (!lastOpenPath.isEmpty()) {
            basePath = lastOpenPath;
        } else {
            basePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
    }

    QString filePath = QFileDialog::getOpenFileName(this, "Import texture", basePath, "*.png");

    if (filePath.isEmpty()) {
        return;
    }

    TextureImportDialog dialog(this);
    dialog.setFilePath(filePath);

    if (dialog.exec() == QDialog::Accepted) {
        mTexturesPtr->push_back(std::move(dialog.getTexture()));
        mModel.setTextures(mTexturesPtr);
    }

    SettingsUtil::SettingsMgr::instance().setLastImportPath(QFileInfo(filePath).absolutePath());
}

void TextureListWidget::exportTexture(Ptcl::Texture* texture) {
    if (!texture) {
        return;
    }

    QString basePath = SettingsUtil::SettingsMgr::instance().lastExportPath();
    if (basePath.isEmpty()) {
        QString lastOpenPath = SettingsUtil::SettingsMgr::instance().lastOpenPath();
        if (!lastOpenPath.isEmpty()) {
            basePath = lastOpenPath;
        } else {
            basePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
    }

    QString filePath = QFileDialog::getSaveFileName(this, "Export texture", basePath, "*.png");

    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(".png")) {
        filePath.append(".png");
    }

    texture->textureData().save(filePath);

    SettingsUtil::SettingsMgr::instance().setLastExportPath(QFileInfo(filePath).absolutePath());
}

void TextureListWidget::replaceTexture(Ptcl::Texture* texture) {
    if (!texture) {
        return;
    }

    QString basePath = SettingsUtil::SettingsMgr::instance().lastImportPath();
    if (basePath.isEmpty()) {
        QString lastOpenPath = SettingsUtil::SettingsMgr::instance().lastOpenPath();
        if (!lastOpenPath.isEmpty()) {
            basePath = lastOpenPath;
        } else {
            basePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
    }

    QString filePath = QFileDialog::getOpenFileName(this, "Import texture", basePath, "*.png");

    if (filePath.isEmpty()) {
        return;
    }

    TextureImportDialog dialog(this);
    dialog.setFilePath(filePath);

    if (dialog.exec() == QDialog::Accepted) {
        texture->replaceTexture(*dialog.getTexture());
        mModel.setTextures(mTexturesPtr);
        mDetailsPanel.setTexture(texture);
    }

    SettingsUtil::SettingsMgr::instance().setLastImportPath(QFileInfo(filePath).absolutePath());

    texture->textureData().save(filePath);
}


// ========================================================================== //


} // namespace PtclEditor
