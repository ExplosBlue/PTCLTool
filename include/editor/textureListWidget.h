#ifndef TEXTURELISTWIDGET_H
#define TEXTURELISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QPushButton>

#include <QLabel>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QScrollArea>
#include <QToolBar>

#include <QtMinMax>

#include "ptcl/ptcl.h"


namespace PtclEditor {


class TextureListItem final : public QWidget {
    Q_OBJECT
public:
    explicit TextureListItem(const QString& text, QIcon thumbnail, QWidget* parent = nullptr);

    // TODO: Enter/Leave events => hide/show buttons

signals:
    void exportImage();

protected:
    void enterEvent(QEnterEvent* event) final;
    void leaveEvent(QEvent* event) final;
    void contextMenuEvent(QContextMenuEvent* event) final;

private:
    QIcon mThumbnail;
    QPushButton* mReplaceButton;
    QPushButton* mRemoveButton;

    QAction mExportAction;
};


// ========================================================================== //


class TextureListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TextureListWidget(QWidget* parent = nullptr);
    void setTextures(Ptcl::TextureList* textures);

protected:
    void resizeEvent(QResizeEvent* event) override;

public slots:
    void exportImage();

private slots:
    void exportAll();

private:
    void populateList();
    void relayoutGrid();

private:
    // This should probably be a weak ptr?
    Ptcl::TextureList* mTexturesPtr;

    QScrollArea mScrollArea;
    QWidget mGridContainer;
    QGridLayout mGridLayout;

    QToolBar mToolbar;
    QAction mActionExportAll;

    std::vector<TextureListItem*> mItemWidgets;
};


// ========================================================================== //


} // namespace PtclEditor


#endif // TEXTURELISTWIDGET_H
