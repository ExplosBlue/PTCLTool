#ifndef PTCLLISTWIDGET_H
#define PTCLLISTWIDGET_H

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include "ptcl/ptcl.h"

namespace PtclEditor {

class PtclList : public QWidget
{
    Q_OBJECT
public:
    explicit PtclList(QWidget *parent = nullptr);

    void setPtclRes(Ptcl::PtclRes *ptclRes);

private:
    void populateList();

private slots:
    void selectionChanged(const QItemSelection& selection);

signals:
    void selectedIndexChanged(u32 index);

private:
    Ptcl::PtclRes* mResPtr; // TODO: this should not be a raw pointer...
    QStandardItemModel mListModel;
    QListView mListView;

    QVBoxLayout mMainLayout;
};

} //namespace PtclEditor

#endif // PTCLLISTWIDGET_H
