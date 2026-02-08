#pragma once

#include "ptcl/ptcl.h"

#include <QAbstractListModel>
#include <QGridLayout>
#include <QListView>
#include <QScrollArea>
#include <QStyledItemDelegate>
#include <QToolBar>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class TextureItemDelegate final : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit TextureItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const final;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const final;

private:
    static constexpr s32 sThumbSize = 64;
    static constexpr s32 sPaddingH = 8;
    static constexpr s32 sPaddingV = 4;
};


// ========================================================================== //


class TextureListModel final : public QAbstractListModel {
    Q_OBJECT
public:
    enum Roles {
        TexturePtrRole = Qt::UserRole + 1,
        FormatRole,
        SizeRole,
        UserCountRole
    };

public:
    explicit TextureListModel(QObject* parent = nullptr);

    void setTextures(Ptcl::TextureList* textures);

    s32 rowCount(const QModelIndex& parent = {}) const final;
    QVariant data(const QModelIndex& index, s32 role) const final;

private:
    void emitRowChangedFor(Ptcl::Texture* texture);

private:
    Ptcl::TextureList* mTextures{nullptr};
};


// ========================================================================== //


class TextureListWidget final : public QWidget {
    Q_OBJECT
public:
    explicit TextureListWidget(QWidget* parent = nullptr);

    void setTextures(Ptcl::TextureList* textures);
    void clear();

private slots:
    void exportAll();
    void importTexture();
    void exportTexture(Ptcl::Texture* texture);
    void replaceTexture(Ptcl::Texture* texture);

private:
    void setupToolbar();
    void setupView();
    void setupContextMenu();

private:
    Ptcl::TextureList* mTexturesPtr{nullptr};

    QToolBar mToolbar{};
    QAction* mActionExportAll{nullptr};
    QAction* mActionImportTexture{nullptr};

    QListView mView{};
    TextureListModel mModel{};
    TextureItemDelegate mDelegate{};
};


// ========================================================================== //


} // namespace PtclEditor
