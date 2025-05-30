#ifndef PTCLLISTWIDGET_H
#define PTCLLISTWIDGET_H

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QLineEdit>

#include "ptcl/ptcl.h"

namespace PtclEditor {

class PtclList : public QWidget {
    Q_OBJECT
public:
    explicit PtclList(QWidget *parent = nullptr);

    void setPtclRes(Ptcl::PtclRes *ptclRes);

private:
    void populateList();

private slots:
    void selectionChanged(const QItemSelection& selection);

    void filterList(const QString& text);

signals:
    void selectedIndexChanged(u32 index);

private:
    Ptcl::PtclRes* mResPtr;
    QStandardItemModel mListModel;
    QListView mListView;
    QLineEdit mSearchBox;

    QVBoxLayout mMainLayout;

    int mSelectedEmitterIndex;
    std::vector<std::pair<u32, QString>> mEmitterNames;
};

} //namespace PtclEditor

#endif // PTCLLISTWIDGET_H
