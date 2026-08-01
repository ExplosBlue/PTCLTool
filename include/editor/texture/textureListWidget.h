#pragma once

#include "editor/texture/textureDetailsPanel.h"
#include "editor/texture/textureFilterPopup.h"
#include "editor/texture/textureFilterProxyModel.h"
#include "editor/texture/textureItemDelegate.h"
#include "editor/texture/textureListModel.h"
#include "ptcl/ptclDocument.h"

#include <QAction>
#include <QListView>
#include <QToolBar>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class TextureListWidget final : public QWidget {
    Q_OBJECT
public:
    explicit TextureListWidget(QWidget* parent = nullptr);

    void setDocument(Ptcl::Document* document);

private slots:
    void exportAll();
    void importTexture();
    void exportTexture(Ptcl::Texture* texture);
    void replaceTexture(const QModelIndex& index);
    void deleteTexture(const QModelIndex& index);

private:
    void setupToolbar();
    void setupView();
    void setupContextMenu();
    void setupLayout();
    void setupSelectionHandling();
    void setupFilterPopup();
    void setFilterButtonChecked(bool checked);

private:
    Ptcl::Document* mDocument{nullptr};

    QToolBar mToolbar{};
    QAction* mActionExportAll{nullptr};
    QAction* mActionImportTexture{nullptr};
    QAction* mFilterAction{nullptr};

    QListView mView{};
    TextureListModel mModel{};
    TextureFilterProxyModel mProxyModel{};
    TextureItemDelegate mDelegate{};

    TextureFilterPopup mFilterPopup{};

    TextureDetailsPanel mDetailsPanel{};
};


// ========================================================================== //


} // namespace PtclEditor
