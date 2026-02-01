#pragma once

#include "ptcl/ptcl.h"

#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit EmitterFilterProxyModel(QObject* parent = nullptr);

protected:
    bool filterAcceptsRow(s32 sourceRow, const QModelIndex& sourceParent) const override;
    QVariant data(const QModelIndex& index, s32 role) const override;
};


// ========================================================================== //


class PtclList : public QWidget {
    Q_OBJECT

public:
    enum class NodeType {
        EmitterSet,
        Emitter,
        ChildData,
        Fluctuation,
        Field
    };

    static constexpr s32 sRoleNodeType = Qt::UserRole;
    static constexpr s32 sRoleSetIdx = Qt::UserRole + 1;
    static constexpr s32 sRoleEmitterIdx = Qt::UserRole + 2;
    static constexpr s32 sRoleEnabled = Qt::UserRole + 3;

public:
    explicit PtclList(QWidget* parent = nullptr);

    void setPtclRes(Ptcl::PtclRes* ptclRes);

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

private slots:
    void selectionChanged(const QItemSelection& selection);
    void filterList(const QString& text);

private:
    void populateList();
    void addComplexNodes(QStandardItem* emitterItem, s32 setIndex, s32 emitterIndex);
    void ensureComplexNode(QStandardItem* emitterItem, NodeType type, const QString& label, s32 setIndex, s32 emitterIndex, bool enabled);

    static QStandardItem* findChildByType(QStandardItem* parent, NodeType type);

private:
    Ptcl::PtclRes* mResPtr{nullptr};

    QStandardItemModel mListModel{};
    QTreeView  mTreeView{};
    QLineEdit mSearchBox{};
    QVBoxLayout mMainLayout{};

    EmitterFilterProxyModel mProxyModel{};
};


// ========================================================================== //


} //namespace PtclEditor

