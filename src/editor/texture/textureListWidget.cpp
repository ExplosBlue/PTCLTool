#include "editor/texture/textureListWidget.h"
#include "editor/texture/textureImportDialog.h"
#include "editor/texture/textureListRoles.h"

#include "util/settingsUtil.h"

#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QStandardPaths>

#include <utility>


namespace PtclEditor {


// ========================================================================== //


TextureListWidget::TextureListWidget(QWidget *parent) :
    QWidget{parent} {
    setupToolbar();
    setupView();
    setupFilterBar();
    setupContextMenu();
    setupLayout();
    setupSelectionHandling();

    connect(&mDetailsPanel, &TextureDetailsPanel::exportRequested, this, &TextureListWidget::exportTexture);
    connect(&mDetailsPanel, &TextureDetailsPanel::replaceRequested, this, &TextureListWidget::replaceTexture);
    connect(&mDetailsPanel, &TextureDetailsPanel::deleteRequested, this, &TextureListWidget::deleteTexture);

    connect(&mModel, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<int>& roles) {
        Q_UNUSED(bottomRight);
        Q_UNUSED(roles);

        if (mDetailsPanel.matchesIndex(topLeft)) {
            mDetailsPanel.refreshTexture();
        }

        mProxyModel.refreshFilter();
    });
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
    mProxyModel.setSourceModel(&mModel);
    mView.setModel(&mProxyModel);
    mView.setItemDelegate(&mDelegate);
}

void TextureListWidget::setupFilterBar() {
    connect(&mFilterBar, &TextureFilterBar::formatsChanged, this,
            [this](const QSet<Ptcl::TextureFormat>& formats) {
        mProxyModel.setEnabledFormats(formats);
    });
    connect(&mFilterBar, &TextureFilterBar::unusedOnlyChanged, this, [this](bool enabled) {
        mProxyModel.setShowUnusedOnly(enabled);
    });
    connect(&mFilterBar, &TextureFilterBar::sizeChanged, this, [this](s32 maxSize) {
        mProxyModel.setMaxSize(maxSize);
    });
    connect(&mFilterBar, &TextureFilterBar::fileSizeChanged, this, [this](s32 maxFileSize) {
        mProxyModel.setMaxFileSize(maxFileSize);
    });
}

void TextureListWidget::setupContextMenu() {
    connect(&mView, &QListView::customContextMenuRequested, this, [this](const QPoint& pos) {
        QModelIndex index = mProxyModel.mapToSource(mView.indexAt(pos));
        if (!index.isValid()) {
            return;
        }

        auto* texture = static_cast<Ptcl::Texture*>(index.data(TextureListRoles::TexturePtrRole).value<void*>());

        if (!texture) {
            return;
        }

        QMenu menu(this);
        menu.addAction("Export", this, [this, texture] {
            exportTexture(texture);
        });
        menu.addAction("Replace", this, [this, index] {
            replaceTexture(index);
        });
        menu.addAction("Delete", this, [this, index] {
            deleteTexture(index);
        });

        menu.exec(mView.viewport()->mapToGlobal(pos));
    });
}

void TextureListWidget::setupLayout() {
    auto* mainLayout = new QVBoxLayout(this);

    auto* listLayout = new QHBoxLayout;
    auto* listColumn = new QVBoxLayout;
    listColumn->setSpacing(2);
    listColumn->addWidget(&mFilterBar);
    listColumn->addWidget(&mView);
    listLayout->addLayout(listColumn);
    listLayout->addWidget(&mDetailsPanel);
    mainLayout->addLayout(listLayout);

    mainLayout->addWidget(&mToolbar);
}

void TextureListWidget::setupSelectionHandling() {
    connect(mView.selectionModel(), &QItemSelectionModel::currentChanged, this, [this](const QModelIndex& current, const QModelIndex& previous) {
        Q_UNUSED(previous);

        const QModelIndex sourceIndex = mProxyModel.mapToSource(current);

        Ptcl::Texture* texture = nullptr;
        if (sourceIndex.isValid()) {
            texture = static_cast<Ptcl::Texture*>(sourceIndex.data(TextureListRoles::TexturePtrRole).value<void*>());
        }
        mDetailsPanel.setTexture(sourceIndex, texture);
    });
}

void TextureListWidget::setDocument(Ptcl::Document* document) {
    if (mDocument) {
        mDocument->disconnect(this);
    }

    mDocument = document;

    if (!mDocument) {
        mModel.setTextures(nullptr);
        mView.selectionModel()->clear();
        mDetailsPanel.setTexture({}, nullptr);

        mActionExportAll->setEnabled(false);
        mActionImportTexture->setEnabled(false);

        setEnabled(false);
        return;
    }

    connect(mDocument, &Ptcl::Document::textureChanged, this, [this](s32 index) {
        QModelIndex idx = mModel.index(index);
        emit mModel.dataChanged(idx, idx);

        if (mDetailsPanel.matchesIndex(idx)) {
            mDetailsPanel.refreshTexture();
        }
    });

    connect(mDocument, &Ptcl::Document::textureAdded, this, [this](s32 index) {
        mModel.onTextureAdded(index);
    });

    connect(mDocument, &Ptcl::Document::textureRemoved, this, [this](s32 index) {
        QModelIndex removedIdx = mModel.index(index);
        if (mDetailsPanel.matchesIndex(removedIdx)) {
            mDetailsPanel.setTexture({}, nullptr);
        }
        mModel.onTextureRemoved(index);
    });

    mModel.setTextures(&mDocument->textures());

    mActionExportAll->setEnabled(true);
    mActionImportTexture->setEnabled(true);

    setEnabled(true);
}

void TextureListWidget::exportAll() {
    if (!mDocument) {
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

    const auto& textures = mDocument->textures();
    for (size_t idx = 0; idx < textures.size(); ++idx) {
        const auto& texture = textures[idx];
        texture->textureData().save(QString("%1/tex_%2.png").arg(dirPath).arg(idx));
    }

    SettingsUtil::SettingsMgr::instance().setLastExportPath(QFileInfo(dirPath).absolutePath());
}

void TextureListWidget::importTexture() {
    if (!mDocument) {
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
        mDocument->addTexture(dialog.getTexture());
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

void TextureListWidget::replaceTexture(const QModelIndex& index) {
    if (!mDocument || !index.isValid()) {
        return;
    }

    const s32 textureIndex = index.row();

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
        auto newTexture = dialog.getTexture();
        mDocument->replaceTexture(textureIndex, std::move(newTexture));
    }

    SettingsUtil::SettingsMgr::instance().setLastImportPath(QFileInfo(filePath).absolutePath());
}

void TextureListWidget::deleteTexture(const QModelIndex& index) {
    if (!mDocument || !index.isValid()) {
        return;
    }

    const s32 textureIndex = index.row();
    const auto& textures = mDocument->textures();
    if (textureIndex < 0 || static_cast<size_t>(textureIndex) >= textures.size()) {
        return;
    }

    const auto* tex = textures[textureIndex].get();
    const auto& img = tex->textureData();
    const u32 users = tex->userCount();

    QString msg = QString("Delete texture?\n\nDimensions: %1x%2\nUsers: %3")
        .arg(img.width()).arg(img.height()).arg(users);

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Delete Texture");
    msgBox.setText(msg);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);
    msgBox.button(QMessageBox::Yes)->setText("Delete");

    if (msgBox.exec() == QMessageBox::Yes) {
        mDocument->removeTexture(textureIndex);
    }
}


// ========================================================================== //


} // namespace PtclEditor
