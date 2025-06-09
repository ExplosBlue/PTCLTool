#pragma once

#include "ptcl/ptcl.h"

#include <QGridLayout>
#include <QScrollArea>
#include <QToolBar>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class TextureListItem final : public QWidget {
    Q_OBJECT
public:
    explicit TextureListItem(const QString& text, QIcon thumbnail, QWidget* parent = nullptr);

signals:
    void exportImage();
    void replaceTexture();

protected:
    void enterEvent(QEnterEvent* event) final;
    void leaveEvent(QEvent* event) final;
    void paintEvent(QPaintEvent* event) final;
    void contextMenuEvent(QContextMenuEvent* event) final;

private:
    QIcon mThumbnail;

    QAction mExportAction;
    QAction mReplaceAction;
    bool mHovered;
};


// ========================================================================== //


class TextureListWidget : public QWidget {
    Q_OBJECT
public:
    explicit TextureListWidget(QWidget* parent = nullptr);
    void setTextures(Ptcl::TextureList* textures);

    void updateItemAt(int index);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void exportAll();
    void exportImage();
    void importTexture();
    void replaceTexture();

private:
    void setupListItem(TextureListItem* item, int index);
    void populateList();
    void relayoutGrid();

private:
    Ptcl::TextureList* mTexturesPtr;

    QScrollArea mScrollArea;
    QWidget mGridContainer;
    QGridLayout mGridLayout;

    QToolBar mToolbar;
    QAction mActionExportAll;
    QAction mActionImportTexture;

    std::vector<TextureListItem*> mItemWidgets;

    s32 mLastColumnCount;
    size_t mLastWidgetCount;
};


// ========================================================================== //


} // namespace PtclEditor
