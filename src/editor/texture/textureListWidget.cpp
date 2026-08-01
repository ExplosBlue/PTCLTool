#include "editor/texture/textureListWidget.h"
#include "editor/texture/textureImportDialog.h"
#include "editor/texture/textureListRoles.h"

#include "util/settingsUtil.h"

#include <QCursor>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QStandardPaths>
#include <QTimer>
#include <QToolButton>

#include <utility>


namespace PtclEditor {


// ========================================================================== //


TextureListWidget::TextureListWidget(QWidget *parent) :
    QWidget{parent} {
    setupToolbar();
    setupView();
    setupFilterPopup();
    setupContextMenu();
    setupLayout();
    setupSelectionHandling();

    connect(&mDetailsPanel, &TextureDetailsPanel::exportRequested, this, &TextureListWidget::exportTexture);
    connect(&mDetailsPanel, &TextureDetailsPanel::replaceRequested, this, &TextureListWidget::replaceTexture);
    connect(&mDetailsPanel, &TextureDetailsPanel::deleteRequested, this, &TextureListWidget::deleteTexture);

    connect(&mModel, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<s32>& roles) {
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

    auto* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mToolbar.addWidget(spacer);

    mFilterAction = mToolbar.addAction(QIcon(":/res/icons/filter.png"), "Filter");
    mFilterAction->setToolTip("Filter textures by format, usage, and size");

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

void TextureListWidget::setupFilterPopup() {
    mFilterPopup.setParent(this, Qt::Popup);

    connect(&mFilterPopup, &TextureFilterPopup::filterChanged, this,
            [this](const TextureFilterState& state) {
        mProxyModel.setEnabledFormats(state.formats);
        mProxyModel.setShowUnusedOnly(state.unusedOnly);
        mProxyModel.setMaxSize(state.maxDimension);
        mProxyModel.setMaxFileSize(state.maxFileSize);
    });

    connect(&mFilterPopup, &TextureFilterPopup::closed, this, [this] {
        setFilterButtonChecked(false);
    });

    connect(mFilterAction, &QAction::triggered, this, [this] {
        if (mFilterPopup.isVisible()) {
            mFilterPopup.hide();
            return;
        }

        const QWidget* anchorWidget = mToolbar.widgetForAction(mFilterAction);
        const QPoint anchor = (anchorWidget && anchorWidget->isVisible())
            ? anchorWidget->mapToGlobal(anchorWidget->rect().center())
            : QCursor::pos();

            mFilterPopup.popup(anchor);
            setFilterButtonChecked(true);
    });
}

void TextureListWidget::setFilterButtonChecked(bool checked) {
    if (auto* button = qobject_cast<QToolButton*>(mToolbar.widgetForAction(mFilterAction))) {
        button->setCheckable(true);
        button->setChecked(checked);
    }
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
    auto* mainLayout = new QHBoxLayout(this);

    auto* listColumn = new QVBoxLayout;
    listColumn->setSpacing(2);
    listColumn->addWidget(&mView, 1);
    listColumn->addWidget(&mToolbar);

    mainLayout->addLayout(listColumn, 1);
    mainLayout->addWidget(&mDetailsPanel);
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
