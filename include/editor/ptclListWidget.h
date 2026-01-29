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
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
};


// ========================================================================== //


class PtclList : public QWidget {
    Q_OBJECT
public:
    explicit PtclList(QWidget* parent = nullptr);

    void setPtclRes(Ptcl::PtclRes* ptclRes);

    void refresh();

signals:
    void selectedEmitterSetChanged(u32 index);
    void selectedEmitterChanged(u32 setIndex, u32 emitterIndex);
    void selectedChildData(u32 setIndex, u32 emitterIndex);
    void selectedFluctuation(u32 setIndex, u32 emitterIndex);

private slots:
    void selectionChanged(const QItemSelection& selection);
    void filterList(const QString& text);

private:
    enum class NodeType {
        EmitterSet,
        Emitter,
        ChildData,
        Fluctuation
    };

    static constexpr s32 sRoleNodeType = Qt::UserRole;
    static constexpr s32 sRoleSetIdx = Qt::UserRole + 1;
    static constexpr s32 sRoleEmitterIdx = Qt::UserRole + 2;

private:
    void populateList();

private:
    Ptcl::PtclRes* mResPtr;

    QStandardItemModel mListModel;
    QTreeView  mTreeView;
    QLineEdit mSearchBox;
    QVBoxLayout mMainLayout;

    EmitterFilterProxyModel mProxyModel;
};


// ========================================================================== //


} //namespace PtclEditor

