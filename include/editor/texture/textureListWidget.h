#pragma once

#include "editor/texture/textureDetailsPanel.h"
#include "editor/texture/textureFilterPopup.h"
#include "editor/texture/textureFilterProxyModel.h"
#include "editor/texture/textureGridDelegate.h"
#include "editor/texture/textureListModel.h"
#include "ptcl/ptclDocument.h"

#include <QAction>
#include <QListView>
#include <QStackedWidget>
#include <QTableView>
#include <QToolBar>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class TextureListWidget final : public QWidget {
    Q_OBJECT
public:
    explicit TextureListWidget(QWidget* parent = nullptr);

    void setDocument(Ptcl::Document* document);
    void setSelection(Ptcl::Selection* selection);

private slots:
    void exportAll();
    void importTexture();
    void exportTexture(Ptcl::Texture* texture);
    void replaceTexture(const QModelIndex& index);
    void deleteTexture(const QModelIndex& index);
    void reEncodeTexture(const QModelIndex& index);

private:
    enum class ViewMode {
        Grid,
        Details
    };

private:
    void setupToolbar();
    void setupViews();
    void setupFilterPopup();
    void setupContextMenus();
    void setupLayout();
    void setupSelectionHandling();

    void switchView(ViewMode mode);
    void showContextMenu(const QPoint& pos, QAbstractItemView* view);
    TextureFilterProxyModel* proxyForView(QAbstractItemView* view);

private:
    Ptcl::Document* mDocument{nullptr};

    QToolBar mToolbar{};
    QAction* mActionExportAll{nullptr};
    QAction* mActionImportTexture{nullptr};
    QAction* mActionViewMode{nullptr};
    QAction* mActionFilter{nullptr};

    QStackedWidget mViewStack{};
    QListView mGridView{};
    QTableView mDetailView{};

    TextureListModel mModel{};
    TextureFilterProxyModel mGridProxy{};
    TextureFilterProxyModel mDetailProxy{};
    TextureGridDelegate mGridDelegate{};
    TextureGridDelegate mDetailThumbDelegate{sDetailCellSize};

    TextureFilterPopup mFilterPopup{};

    TextureDetailsPanel mDetailsPanel{};

    static constexpr s32 sDetailCellSize = 64;
};


// ========================================================================== //


} // namespace PtclEditor
