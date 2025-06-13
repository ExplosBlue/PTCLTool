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

private slots:
    void selectionChanged(const QItemSelection& selection);
    void filterList(const QString& text);

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

