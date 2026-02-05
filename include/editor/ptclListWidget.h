#pragma once

#include "util/bitflagUtil.h"
#include "ptcl/ptcl.h"

#include <QLineEdit>
#include <QMenu>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTreeView>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QToolBar>


namespace PtclEditor {


// ========================================================================== //


enum class NodeType {
    EmitterSet,
    Emitter,
    ChildData,
    Fluctuation,
    Field
};

enum class EmitterFilterFlag {
    Simple  = 1 << 0,
    Complex = 1 << 1,
    Compact = 1 << 2,
};

using EmitterFilter = BitFlag<EmitterFilterFlag>;

static constexpr s32 sRoleNodeType      = Qt::UserRole;
static constexpr s32 sRoleSetIdx        = Qt::UserRole + 1;
static constexpr s32 sRoleEmitterIdx    = Qt::UserRole + 2;
static constexpr s32 sRoleEnabled       = Qt::UserRole + 3;
static constexpr s32 sRoleEmitterType   = Qt::UserRole + 4;


// ========================================================================== //


class EmitterFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit EmitterFilterProxyModel(QObject* parent = nullptr);

    void setEmitterFilter(const EmitterFilter& filter);

protected:
    bool filterAcceptsRow(s32 sourceRow, const QModelIndex& sourceParent) const override;
    QVariant data(const QModelIndex& index, s32 role) const override;

private:
    EmitterFilter mEmitterFilter{EmitterFilterFlag::Simple, EmitterFilterFlag::Complex, EmitterFilterFlag::Compact};
};


// ========================================================================== //


class PtclList : public QWidget {
    Q_OBJECT
public:
    explicit PtclList(QWidget* parent = nullptr);

    void setPtclRes(Ptcl::PtclRes* ptclRes);

    void selectEmitter(s32 setIndex, s32 emitterIndex);

    void updateEmitter(s32 setIndex, s32 emitterIndex);
    void updateEmitterName(s32 setIndex, s32 emitterIndex);
    void updateEmitterSetName(s32 setIndex);
    void refresh();

signals:
    void selectedEmitterSetChanged(u32 index);
    void selectedEmitterChanged(u32 setIndex, u32 emitterIndex);
    void selectedChildData(u32 setIndex, u32 emitterIndex);
    void selectedFluctuation(u32 setIndex, u32 emitterIndex);
    void selectedField(u32 setIndex, u32 emitterIndex);

    void emitterAdded(s32 setIndex, s32 emitterIndex);
    void emitterSetAdded(s32 setIndex);

    void emitterRemoved(s32 setIndex, s32 newEmitterIndex);
    void emitterSetRemoved(s32 setIndex);

private slots:
    void selectionChanged(const QItemSelection& selection);
    void filterList(const QString& text);

private:
    void populateList();
    void setupFilterMenu();

    void addComplexNodes(QStandardItem* emitterItem, s32 setIndex, s32 emitterIndex);
    void ensureComplexNode(QStandardItem* emitterItem, NodeType type, const QString& label, s32 setIndex, s32 emitterIndex, bool enabled);

    void updateToolbarForSelection(const QStandardItem* item);

    static QStandardItem* findChildByType(QStandardItem* parent, NodeType type);

    void insertEmitterNode(QStandardItem* setItem, s32 setIndex, s32 emitterIndex);
    void insertEmitterSetNode(s32 setIndex);

    void addEmitter();
    void addEmitterSet();

    void removeItem();
    void removeEmitter(QStandardItem* setItem, QStandardItem* emitterItem);
    void removeEmitterSet(QStandardItem* setItem);

    void reindexEmitters(QStandardItem* setItem, s32 setIndex);
    void reindexEmitterSets();

private:
    Ptcl::PtclRes* mResPtr{nullptr};

    QStandardItemModel mListModel{};
    QTreeView  mTreeView{};
    QLineEdit mSearchBox{};
    QToolButton mFilterButton{};
    QMenu mFilterMenu{};

    QToolBar mToolBar{};
    QAction* mAddEmitterSetAction{nullptr};
    QAction* mAddEmitterAction{nullptr};
    QAction* mRemoveAction{nullptr};
    // QAction* mCopyAction{nullptr};
    // QAction* mPasteAction{nullptr};

    QVBoxLayout mMainLayout{};

    EmitterFilterProxyModel mProxyModel{};
};


// ========================================================================== //


} //namespace PtclEditor

