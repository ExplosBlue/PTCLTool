#pragma once

#include "editor/components/thumbnailWidget.h"
#include "ptcl/ptcl.h"

#include <QAbstractListModel>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <QStyledItemDelegate>


// ========================================================================== //


class TextureSelectModel final : public QAbstractListModel {
    Q_OBJECT
public:
    explicit TextureSelectModel(QObject* parent = nullptr);

    void setTextures(const Ptcl::TextureList* textures, s32 filterFormat = -1);

    s32 rowCount(const QModelIndex& parent = {}) const final;
    QVariant data(const QModelIndex& index, s32 role) const final;

    s32 globalIndex(s32 row) const;

private:
    struct Entry {
        s32 globalIndex;
    };

private:
    void rebuildFilter(s32 filterFormat);

private:
    const Ptcl::TextureList* mTextures{nullptr};
    std::vector<Entry> mEntries{};
};


// ========================================================================== //


class TextureSelectDelegate final : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit TextureSelectDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const final;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const final;

private:
    static constexpr s32 sThumbSize = 96;
    static constexpr s32 sPadding = 8;
};


// ========================================================================== //


class TextureSelectDialog : public QDialog {
    Q_OBJECT
public:
    explicit TextureSelectDialog(const Ptcl::TextureList& textures, QWidget* parent = nullptr);
    int selectedIndex() const;

private slots:
    void onSelectionChanged(const QModelIndex& current, const QModelIndex& previous);

private:
    void clearSelection();

private:
    const Ptcl::TextureList& mTextures;

    QComboBox mFilterCombo{};

    QListView mListView{};
    TextureSelectModel mModel{};
    TextureSelectDelegate mDelegate{};

    ThumbnailWidget mPreviewWidget{};
    QLabel mNameLabel{};
    QLabel mFormatLabel{};
    QLabel mDimensionsLabel{};
    QLabel mSizeLabel{};
    QLabel mUsersLabel{};

    QPushButton* mOkButton{nullptr};
    int mSelectedIndex{-1};
};


// ========================================================================== //
