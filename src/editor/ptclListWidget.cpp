#include "editor/ptclListWidget.h"


namespace PtclEditor {


// ========================================================================== //


PtclList::PtclList(QWidget* parent) :
    QWidget(parent), mResPtr(nullptr) {
    // Search Box
    mSearchBox.setPlaceholderText("Search");
    connect(&mSearchBox, &QLineEdit::textChanged, this, &PtclList::filterList);

    // List View
    mListView.setModel(&mListModel);
    connect(mListView.selectionModel(), &QItemSelectionModel::selectionChanged, this, &PtclList::selectionChanged);

    // Main layout
    mMainLayout.addWidget(&mListView);
    mMainLayout.addWidget(&mSearchBox);

    setLayout(&mMainLayout);
}

void PtclList::setPtclRes(Ptcl::PtclRes* ptclRes) {
    if (mResPtr == ptclRes) {
        return;
    }

    mListModel.clear();

    mResPtr = ptclRes;

    populateList();
}

void PtclEditor::PtclList::refreshNames() {
    populateList();
}

void PtclList::populateList() {
    if (!mResPtr) {
        return;
    }

    mEmitterNames.clear();

    u32 index = 0;
    for (auto& emitterSet : mResPtr->getEmitterSets()) {
        QString name = QString::number(index) + ": " + emitterSet->name();
        mEmitterNames.emplace_back(index, name);
        ++index;
    }

    filterList(mSearchBox.text());
}

void PtclList::filterList(const QString& text) {
    mListModel.clear();

    int rowToSelect = -1;
    int row = 0;

    for (const auto& [index, name] : mEmitterNames) {
        if (text.isEmpty() || name.contains(text, Qt::CaseInsensitive)) {
            QStandardItem* item = new QStandardItem(name);
            item->setEditable(false);
            item->setData(index, Qt::UserRole);
            mListModel.appendRow(item);

            if (index == mSelectedEmitterIndex) {
                rowToSelect = row;
            }
            ++row;
        }
    }

    if (rowToSelect >= 0) {
        QModelIndex indexToSelect = mListModel.index(rowToSelect, 0);
        mListView.setCurrentIndex(indexToSelect);
        mListView.selectionModel()->select(indexToSelect, QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
}

void PtclList::selectionChanged(const QItemSelection& selection) {
    if (selection.indexes().isEmpty())
        return;

    QModelIndex index = selection.indexes().front();
    mSelectedEmitterIndex = index.data(Qt::UserRole).toInt();
    emit selectedIndexChanged(mSelectedEmitterIndex);
}


// ========================================================================== //


} // namespace PtclEditor
