#pragma once

#include "util/bitflagUtil.h"
#include "ptcl/ptclDocument.h"
#include "editor/emitterList/emitterListTypes.h"
#include "editor/emitterList/emitterFilterMenu.h"
#include "editor/emitterList/emitterFilterProxyModel.h"
#include "editor/emitterList/emitterListController.h"
#include "editor/emitterList/emitterListContextMenu.h"

#include <QLineEdit>
#include <QShortcut>
#include <QStandardItemModel>
#include <QTreeView>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QToolBar>


namespace PtclTool {


// ========================================================================== //


class EmitterList : public QWidget {
    Q_OBJECT
public:
    explicit EmitterList(QWidget* parent = nullptr);

    void setDocument(Ptcl::Document* document);
    void setSelection(Ptcl::Selection* selection);

private slots:
    void filterList(const QString& text);

private:
    void showContextMenu(const QPoint& pos);

    void applyIcons();

    QIcon nodeIcon(NodeType type) const;

    void updateToolbarForSelection(const QStandardItem* item);

    QStandardItem* currentItem() const;
    QStandardItem* contextItemForSelection() const;

private:
    Ptcl::Document* mDocument{nullptr};
    Ptcl::Selection* mSelection{nullptr};

    QTreeView  mTreeView{};
    QLineEdit mSearchBox{};
    QToolButton mFilterButton{};
    EmitterFilterMenu mFilterMenu{};

    QToolBar mToolBar{};
    QAction* mAddEmitterSetAction{nullptr};
    QAction* mAddEmitterAction{nullptr};
    QAction* mRemoveAction{nullptr};
    QAction* mCopyAction{nullptr};
    QAction* mPasteAction{nullptr};

    QVBoxLayout mMainLayout{};

    EmitterFilterProxyModel mProxyModel{};
    EmitterListController mListController{};
    EmitterListContextMenu mContextMenu{this};

    QShortcut mCopyShortcut{QKeySequence::Copy, this};
    QShortcut mPasteShortcut{QKeySequence::Paste, this};
    QShortcut mDuplicateShortcut{QKeySequence(Qt::CTRL | Qt::Key_D), this};
};


// ========================================================================== //


} //namespace PtclTool
