#include "ptclList.h"

PtclList::PtclList(Ptcl::PtclRes* res, QWidget* parent) :
    QWidget(parent), mRes(res) {

    u32 index = 0;
    for (auto& emitterSet : res->getEmitterSets()) {
        QStandardItem* item = new QStandardItem();
        item->setText(emitterSet.name());
        item->setEditable(false);
        item->setData(index);
        mListModel.appendRow(item);
        ++index;
    }

    mListView.setModel(&mListModel);

    mMainLayout.addWidget(&mListView);
    setLayout(&mMainLayout);

    connect(mListView.selectionModel(), &QItemSelectionModel::selectionChanged, this, &PtclList::selectionChanged);
}


void PtclList::selectionChanged(const QItemSelection& selection) {

    u32 emitterSetIdx = selection.indexes().front().row();
    emit selectedIndexChanged(emitterSetIdx);
}
