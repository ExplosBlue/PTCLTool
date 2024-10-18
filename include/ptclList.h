#ifndef PTCLLIST_H
#define PTCLLIST_H

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include "ptcl/ptcl.h"

class PtclList : public QWidget
{
    Q_OBJECT
public:
    explicit PtclList(Ptcl::PtclRes *ptclRes, QWidget *parent = nullptr);

private slots:
    void selectionChanged(const QItemSelection& selection);

signals:
    void selectedIndexChanged(u32 index);

private:
    Ptcl::PtclRes* mRes; // TODO: this should not be a raw pointer...
    QStandardItemModel mListModel;
    QListView mListView;

    QVBoxLayout mMainLayout;
};

#endif // PTCLLIST_H
