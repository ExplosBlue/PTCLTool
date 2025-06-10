#ifndef PTCLLISTWIDGET_H
#define PTCLLISTWIDGET_H

#include "ptcl/ptcl.h"

#include <QLineEdit>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class PtclList : public QWidget {
    Q_OBJECT
public:
    explicit PtclList(QWidget *parent = nullptr);

    void setPtclRes(Ptcl::PtclRes *ptclRes);

    void refreshNames();

signals:
    void selectedIndexChanged(u32 index);

private slots:
    void selectionChanged(const QItemSelection& selection);
    void filterList(const QString& text);

private:
    void populateList();

private:
    Ptcl::PtclRes* mResPtr;

    QStandardItemModel mListModel;
    QListView mListView;
    QLineEdit mSearchBox;
    QVBoxLayout mMainLayout;

    int mSelectedEmitterIndex;
    std::vector<std::pair<u32, QString>> mEmitterNames;
};


// ========================================================================== //


} //namespace PtclEditor


#endif // PTCLLISTWIDGET_H
