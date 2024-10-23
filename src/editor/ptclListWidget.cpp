#include "editor/ptclListWidget.h"

namespace PtclEditor {

PtclList::PtclList(QWidget* parent) :
    QWidget(parent), mResPtr(nullptr) {

    mListView.setModel(&mListModel);

    mMainLayout.addWidget(&mListView);
    setLayout(&mMainLayout);

    connect(mListView.selectionModel(), &QItemSelectionModel::selectionChanged, this, &PtclList::selectionChanged);
}

void PtclList::setPtclRes(Ptcl::PtclRes* ptclRes) {

    if (mResPtr == ptclRes) {
        return;
    }

    mListModel.clear();

    mResPtr = ptclRes;

    populateList();
}

void PtclList::populateList() {

    if (!mResPtr) {
        return;
    }

    mListModel.clear();

    u32 index = 0;
    for (auto& emitterSet : mResPtr->getEmitterSets()) {
        QStandardItem* item = new QStandardItem();
        item->setText(emitterSet->name());
        item->setEditable(false);
        item->setData(index);
        mListModel.appendRow(item);
        ++index;
    }
}

void PtclList::selectionChanged(const QItemSelection& selection) {

    u32 emitterSetIdx = selection.indexes().front().row();
    emit selectedIndexChanged(emitterSetIdx);
}

} // namespace PtclEditor
