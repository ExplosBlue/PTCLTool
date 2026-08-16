#include "editor/texture/textureListWidget.h"
#include "editor/texture/textureImportDialog.h"
#include "editor/texture/textureListRoles.h"

#include "util/settingsUtil.h"
#include "util/iconUtil.h"

#include <QAbstractItemView>
#include <QActionGroup>
#include <QFileDialog>
#include <QHeaderView>
#include <QMenu>
#include <QMessageBox>
#include <QStyle>
#include <QToolButton>

#include <utility>


namespace PtclEditor {


// ========================================================================== //


TextureListWidget::TextureListWidget(QWidget *parent) :
    QWidget{parent} {
    setupToolbar();
    setupViews();
    setupFilterPopup();
    setupContextMenus();
    setupLayout();
    setupSelectionHandling();
    applyIcons();

    connect(&mDetailsPanel, &TextureDetailsPanel::exportRequested, this, &TextureListWidget::exportTexture);
    connect(&mDetailsPanel, &TextureDetailsPanel::replaceRequested, this, &TextureListWidget::replaceTexture);
    connect(&mDetailsPanel, &TextureDetailsPanel::deleteRequested, this, &TextureListWidget::deleteTexture);

    connect(&mModel, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex& topLeft, const QModelIndex& bottomRight, const QList<s32>& roles) {
        Q_UNUSED(bottomRight);
        Q_UNUSED(roles);

        if (mDetailsPanel.matchesIndex(topLeft)) {
            mDetailsPanel.refresh();
        }

        mGridProxy.refreshFilter();
        mDetailProxy.refreshFilter();

        if (mDetailProxy.sortColumn() >= 0) {
            mDetailProxy.sort(mDetailProxy.sortColumn(), mDetailProxy.sortOrder());
        }
    });

    connect(&IconManager::instance(), &IconManager::iconsChanged, this, &TextureListWidget::applyIcons);
}

void TextureListWidget::setupToolbar() {
    mToolbar.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mActionExportAll = mToolbar.addAction("Export All");
    mActionImportTexture = mToolbar.addAction("Import Texture");

    mToolbar.addSeparator();

    auto* spacer = new QWidget(this);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    mToolbar.addWidget(spacer);

    auto* viewMenu = new QMenu(&mToolbar);
    auto* viewGroup = new QActionGroup(viewMenu);
    viewGroup->setExclusive(true);

    auto* gridAction = viewMenu->addAction("Grid");
    auto* detailsAction = viewMenu->addAction("Details");

    gridAction->setCheckable(true);
    detailsAction->setCheckable(true);
    detailsAction->setChecked(true);

    viewGroup->addAction(gridAction);
    viewGroup->addAction(detailsAction);

    connect(gridAction, &QAction::triggered, this, [this] {
        mViewMode = ViewMode::Grid;
        mActionViewMode->setText("Grid");
        switchView(mViewMode);
        applyIcons();
    });

    connect(detailsAction, &QAction::triggered, this, [this] {
        mViewMode = ViewMode::Details;
        mActionViewMode->setText("Details");
        switchView(mViewMode);
        applyIcons();
    });

    mActionViewMode = mToolbar.addAction("Details");
    mActionViewMode->setIcon(mToolbar.style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    mActionViewMode->setMenu(viewMenu);

    if (auto* viewButton = qobject_cast<QToolButton*>(mToolbar.widgetForAction(mActionViewMode))) {
        viewButton->setPopupMode(QToolButton::InstantPopup);
    }

    mActionFilter = mToolbar.addAction("Filter");
    mActionFilter->setToolTip("Filter textures by format, usage, and size");

    connect(mActionExportAll, &QAction::triggered, this, &TextureListWidget::exportAll);
    connect(mActionImportTexture, &QAction::triggered, this, &TextureListWidget::importTexture);
}

void TextureListWidget::setupViews() {
    mGridView.setViewMode(QListView::IconMode);
    mGridView.setResizeMode(QListView::Adjust);
    mGridView.setSpacing(10);
    mGridView.setUniformItemSizes(true);
    mGridView.setMouseTracking(true);
    mGridView.setSelectionMode(QAbstractItemView::SingleSelection);
    mGridView.setContextMenuPolicy(Qt::CustomContextMenu);
    mGridView.setItemDelegate(&mGridDelegate);
    mGridView.setModel(&mGridProxy);

    mDetailView.setSelectionMode(QAbstractItemView::SingleSelection);
    mDetailView.setSelectionBehavior(QAbstractItemView::SelectRows);
    mDetailView.setContextMenuPolicy(Qt::CustomContextMenu);
    mDetailView.setModel(&mDetailProxy);
    mDetailView.setSortingEnabled(true);
    mDetailView.setItemDelegateForColumn(TextureColumn::ThumbnailColumn, &mDetailThumbDelegate);
    mDetailView.verticalHeader()->setVisible(true);
    mDetailView.verticalHeader()->setDefaultSectionSize(sDetailCellSize);
    mDetailView.verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    mDetailView.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mGridProxy.setSourceModel(&mModel);
    mDetailProxy.setSourceModel(&mModel);
    mDetailView.sortByColumn(TextureColumn::ThumbnailColumn, Qt::AscendingOrder);

    mViewStack.addWidget(&mGridView);
    mViewStack.addWidget(&mDetailView);
    mViewStack.setCurrentWidget(&mDetailView);
}

void TextureListWidget::setupFilterPopup() {
    mFilterPopup.setParent(this, Qt::Popup);

    connect(&mFilterPopup, &TextureFilterPopup::filterChanged, this,
            [this](const TextureFilterState& state) {
        mGridProxy.setEnabledFormats(state.formats);
        mGridProxy.setShowUnusedOnly(state.unusedOnly);
        mGridProxy.setMaxSize(state.maxDimension);
        mGridProxy.setMaxFileSize(state.maxFileSize);

        mDetailProxy.setEnabledFormats(state.formats);
        mDetailProxy.setShowUnusedOnly(state.unusedOnly);
        mDetailProxy.setMaxSize(state.maxDimension);
        mDetailProxy.setMaxFileSize(state.maxFileSize);
    });

    mActionFilter->setMenu(&mFilterPopup);
    if (auto* button = qobject_cast<QToolButton*>(mToolbar.widgetForAction(mActionFilter))) {
        button->setPopupMode(QToolButton::InstantPopup);
    }
}

void TextureListWidget::setupContextMenus() {
    auto connectContextMenu = [this](QAbstractItemView* view) {
        connect(view, &QWidget::customContextMenuRequested, this, [this, view](const QPoint& pos) {
            showContextMenu(pos, view);
        });
    };

    connectContextMenu(&mGridView);
    connectContextMenu(&mDetailView);
}

void TextureListWidget::setupLayout() {
    auto* mainLayout = new QHBoxLayout(this);

    auto* listColumn = new QVBoxLayout;
    listColumn->setSpacing(2);
    listColumn->addWidget(&mViewStack, 1);
    listColumn->addWidget(&mToolbar);

    mainLayout->addLayout(listColumn, 1);
    mainLayout->addWidget(&mDetailsPanel);
}

void TextureListWidget::setupSelectionHandling() {
    auto onCurrentChanged = [this](TextureFilterProxyModel* proxy, const QModelIndex& current, const QModelIndex& previous) {
        Q_UNUSED(previous);

        QModelIndex sourceIndex = proxy->mapToSource(current);
        if (sourceIndex.isValid()) {
            sourceIndex = mModel.index(sourceIndex.row(), TextureColumn::ThumbnailColumn);
        }

        Ptcl::Texture* texture = nullptr;
        if (sourceIndex.isValid()) {
            texture = static_cast<Ptcl::Texture*>(sourceIndex.data(TextureListRoles::TexturePtrRole).value<void*>());
        }
        mDetailsPanel.setTexture(sourceIndex, texture);
    };

    connect(mGridView.selectionModel(), &QItemSelectionModel::currentChanged, this,
            [this, onCurrentChanged](const QModelIndex& current, const QModelIndex& previous) {
        onCurrentChanged(&mGridProxy, current, previous);
    });

    connect(mDetailView.selectionModel(), &QItemSelectionModel::currentChanged, this,
            [this, onCurrentChanged](const QModelIndex& current, const QModelIndex& previous) {
        onCurrentChanged(&mDetailProxy, current, previous);
    });
}

void TextureListWidget::applyIcons() {
    constexpr QSize iconSize{16, 16};
    IconUtil::setIcon(mActionExportAll, "export", this, iconSize);
    IconUtil::setIcon(mActionImportTexture, "import", this,iconSize);
    IconUtil::setIcon(mActionFilter, "filter", this, iconSize);

    if (mViewMode == ViewMode::Grid) {
        IconUtil::setIcon(mActionViewMode, "grid", this, iconSize);
    } else {
        IconUtil::setIcon(mActionViewMode, "details", this, iconSize);
    }
}

void TextureListWidget::setSelection(Ptcl::Selection* selection) {
    mDetailsPanel.setSelection(selection);
}

void TextureListWidget::switchView(ViewMode mode) {
    mViewStack.setCurrentWidget(mode == ViewMode::Grid ? static_cast<QWidget*>(&mGridView) : static_cast<QWidget*>(&mDetailView));

    QAbstractItemView* target = mode == ViewMode::Grid ? static_cast<QAbstractItemView*>(&mGridView) : static_cast<QAbstractItemView*>(&mDetailView);
    QAbstractItemView* other = mode == ViewMode::Grid ? static_cast<QAbstractItemView*>(&mDetailView) : static_cast<QAbstractItemView*>(&mGridView);

    const QModelIndex current = other->selectionModel()->currentIndex();
    if (current.isValid()) {
        const QModelIndex source = proxyForView(other)->mapToSource(current);
        const QModelIndex mapped = proxyForView(target)->mapFromSource(source);
        target->selectionModel()->setCurrentIndex(mapped, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Current);
        target->scrollTo(mapped);
    }
}

TextureFilterProxyModel* TextureListWidget::proxyForView(QAbstractItemView* view) {
    if (view == &mGridView) {
        return &mGridProxy;
    }
    return &mDetailProxy;
}

void TextureListWidget::showContextMenu(const QPoint& pos, QAbstractItemView* view) {
    const QModelIndex index = proxyForView(view)->mapToSource(view->indexAt(pos));
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

    menu.addAction("Re-encode", this, [this, index] {
        reEncodeTexture(index);
    });

    menu.addSeparator();

    menu.addAction("Delete", this, [this, index] {
        deleteTexture(index);
    });

    menu.exec(view->viewport()->mapToGlobal(pos));
}

void TextureListWidget::setDocument(Ptcl::Document* document) {
    if (mDocument) {
        mDocument->disconnect(this);
    }

    mDocument = document;
    mDetailsPanel.setDocument(document);

    if (!mDocument) {
        mModel.setTextures(nullptr);
        mGridView.selectionModel()->clearSelection();
        mGridView.selectionModel()->clearCurrentIndex();
        mDetailView.selectionModel()->clearSelection();
        mDetailView.selectionModel()->clearCurrentIndex();
        mDetailsPanel.setTexture({}, nullptr);

        mActionExportAll->setEnabled(false);
        mActionImportTexture->setEnabled(false);

        setEnabled(false);
        return;
    }

    connect(mDocument, &Ptcl::Document::textureChanged, this, [this](s32 index) {
        const QModelIndex topLeft = mModel.index(index, TextureColumn::ThumbnailColumn);
        const QModelIndex bottomRight = mModel.index(index, TextureColumn::TextureColumnCount - 1);
        emit mModel.dataChanged(topLeft, bottomRight);
    });

    connect(mDocument, &Ptcl::Document::textureAdded, this, [this](s32 index) {
        mModel.onTextureAdded(index);
    });

    connect(mDocument, &Ptcl::Document::textureRemoved, this, [this](s32 index) {
        const QModelIndex removedIdx = mModel.index(index, TextureColumn::ThumbnailColumn);
        if (mDetailsPanel.matchesIndex(removedIdx)) {
            mDetailsPanel.setTexture({}, nullptr);
        }
        mModel.onTextureRemoved(index);
    });

    connect(mDocument, &Ptcl::Document::emitterChanged, this, [this](s32 setIndex, s32 emitterIndex) {
        Q_UNUSED(setIndex);
        Q_UNUSED(emitterIndex);
        mDetailsPanel.refreshUsers();
    });

    connect(mDocument, &Ptcl::Document::emitterAdded, this, [this](s32 setIndex, s32 emitterIndex) {
        Q_UNUSED(setIndex);
        Q_UNUSED(emitterIndex);
        mDetailsPanel.refreshUsers();
    });

    connect(mDocument, &Ptcl::Document::emitterRemoved, this, [this](s32 setIndex, s32 emitterIndex) {
        Q_UNUSED(setIndex);
        Q_UNUSED(emitterIndex);
        mDetailsPanel.refreshUsers();
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

    QString dirPath = QFileDialog::getExistingDirectory(this, "Export textures",
        SettingsUtil::dialogPath(SettingsUtil::PathType::Export),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dirPath.isEmpty()) {
        return;
    }

    const auto& textures = mDocument->textures();
    for (size_t idx = 0; idx < textures.size(); ++idx) {
        const auto& texture = textures[idx];
        texture->textureData().save(QString("%1/tex_%2.png").arg(dirPath).arg(idx));
    }

    SettingsUtil::setDialogPath(SettingsUtil::PathType::Export, dirPath);
}

void TextureListWidget::importTexture() {
    if (!mDocument) {
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(this, "Import texture",
        SettingsUtil::dialogPath(SettingsUtil::PathType::Import),
        "*.png");

    if (filePath.isEmpty()) {
        return;
    }

    TextureImportDialog dialog(this);

    const auto image = QImage(filePath);
    dialog.setImage(image);

    if (dialog.exec() == QDialog::Accepted) {
        mDocument->addTexture(dialog.getTexture());
    }

    SettingsUtil::setDialogPath(SettingsUtil::PathType::Import, filePath);
}

void TextureListWidget::exportTexture(Ptcl::Texture* texture) {
    if (!texture) {
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, "Export texture",
        SettingsUtil::dialogPath(SettingsUtil::PathType::Export),
        "*.png");

    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(".png")) {
        filePath.append(".png");
    }

    texture->textureData().save(filePath);

    SettingsUtil::setDialogPath(SettingsUtil::PathType::Export, filePath);
}

void TextureListWidget::replaceTexture(const QModelIndex& index) {
    if (!mDocument || !index.isValid()) {
        return;
    }

    const s32 textureIndex = index.row();

    QString filePath = QFileDialog::getOpenFileName(this, "Import texture",
        SettingsUtil::dialogPath(SettingsUtil::PathType::Import),
        "*.png");

    if (filePath.isEmpty()) {
        return;
    }

    TextureImportDialog dialog(this);

    const auto image = QImage(filePath);
    dialog.setImage(image);

    if (dialog.exec() == QDialog::Accepted) {
        auto newTexture = dialog.getTexture();
        mDocument->replaceTexture(textureIndex, std::move(newTexture), "Replace Texture");
    }

    SettingsUtil::setDialogPath(SettingsUtil::PathType::Import, filePath);
}

void TextureListWidget::reEncodeTexture(const QModelIndex& index) {
    if (!mDocument || !index.isValid()) {
        return;
    }

    const s32 textureIndex = index.row();
    const auto& texture = mDocument->textures()[textureIndex]->textureData();

    TextureImportDialog dialog(this);
    dialog.setImage(texture);

    if (dialog.exec() == QDialog::Accepted) {
        auto newTexture = dialog.getTexture();
        mDocument->replaceTexture(textureIndex, std::move(newTexture), "Re-encode Texture");
    }
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
