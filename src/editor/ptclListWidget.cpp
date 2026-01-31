#include "editor/ptclListWidget.h"


namespace PtclEditor {


// ========================================================================== //


EmitterFilterProxyModel::EmitterFilterProxyModel(QObject* parent) :
    QSortFilterProxyModel{parent} {}

bool EmitterFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const {
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (!index.isValid()) {
        return false;
    }

    if (sourceModel()->data(index).toString().contains(filterRegularExpression())) {
        return true;
    }

    for (int i = 0; i < sourceModel()->rowCount(index); ++i) {
        if (filterAcceptsRow(i, index)) {
            return true;
        }
    }

    return false;
}


// ========================================================================== //


PtclList::PtclList(QWidget* parent) :
    QWidget(parent), mResPtr(nullptr) {
    // Search Box
    mSearchBox.setPlaceholderText("Search");
    connect(&mSearchBox, &QLineEdit::textChanged, this, &PtclList::filterList);

    // Proxy Model
    mProxyModel.setSourceModel(&mListModel);
    mProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    mProxyModel.setRecursiveFilteringEnabled(true);

    // Tree View
    mTreeView.setModel(&mProxyModel);
    mTreeView.setHeaderHidden(true);
    connect(mTreeView.selectionModel(), &QItemSelectionModel::selectionChanged, this, &PtclList::selectionChanged);

    // Main layout
    mMainLayout.addWidget(&mTreeView);
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

void PtclEditor::PtclList::refresh() {
    populateList();
}

void PtclList::populateList() {
    if (!mResPtr) {
        return;
    }

    mListModel.clear();

    // EmitterSets
    const auto& sets = mResPtr->getEmitterSets();
    for (u32 setIndex = 0; setIndex < sets.size(); ++setIndex) {
        const auto& set = sets[setIndex];

        QString setName = QString("%1: %2").arg(setIndex).arg(set->name());
        auto* setItem = new QStandardItem(setName);
        setItem->setEditable(false);
        setItem->setSelectable(false);
        setItem->setData(static_cast<s32>(NodeType::EmitterSet), sRoleNodeType);
        setItem->setData(setIndex, sRoleSetIdx);

        // Emitters
        for (u32 emitterIndex = 0; emitterIndex < set->emitters().size(); ++emitterIndex) {
            auto* emitter = set->emitters()[emitterIndex].get();

            QString emitterName = QString("Emitter %1: %2").arg(emitterIndex).arg(emitter->name());
            auto* emitterItem = new QStandardItem(emitterName);
            emitterItem->setEditable(false);
            emitterItem->setData(static_cast<s32>(NodeType::Emitter), sRoleNodeType);
            emitterItem->setData(setIndex, sRoleSetIdx);
            emitterItem->setData(emitterIndex, sRoleEmitterIdx);

            // Complex Data
            if (emitter->type() == Ptcl::EmitterType::Complex || emitter->type() == Ptcl::EmitterType::UnkType2) {
                addComplexNodes(emitterItem, setIndex, emitterIndex);
            }
            setItem->appendRow(emitterItem);
        }
        mListModel.appendRow(setItem);
    }

    mTreeView.expandAll();
    filterList(mSearchBox.text());
}

void PtclList::filterList(const QString& text) {
    mProxyModel.setFilterFixedString(text);
}

void PtclList::selectionChanged(const QItemSelection& selection) {
    if (selection.indexes().isEmpty()) {
        return;
    }

    QModelIndex proxyIndex = selection.indexes().first();
    QModelIndex sourceIndex = mProxyModel.mapToSource(proxyIndex);
    QStandardItem* item = mListModel.itemFromIndex(sourceIndex);

    if (!item) {
        return;
    }

    auto type = static_cast<NodeType>(item->data(sRoleNodeType).toUInt());
    u32 setIndex = item->data(sRoleSetIdx).toUInt();
    u32 emitterIndex = item->data(sRoleEmitterIdx).toUInt();

    switch (type) {
    case NodeType::EmitterSet:
        emit selectedEmitterSetChanged(setIndex);
        break;
    case NodeType::Emitter:
        emit selectedEmitterChanged(setIndex, emitterIndex);
        break;
    case NodeType::ChildData:
        emit selectedChildData(setIndex, emitterIndex);
        break;
    case NodeType::Fluctuation:
        emit selectedFluctuation(setIndex, emitterIndex);
        break;
    case NodeType::Field:
        emit selectedField(setIndex, emitterIndex);
        break;
    }
}

void PtclList::addComplexNodes(QStandardItem* emitterItem, s32 setIndex, s32 emitterIndex) {
    // ChildData
    auto* childItem = new QStandardItem("ChildData");
    childItem->setEditable(false);
    childItem->setData(static_cast<s32>(NodeType::ChildData), sRoleNodeType);
    childItem->setData(setIndex, sRoleSetIdx);
    childItem->setData(emitterIndex, sRoleEmitterIdx);
    emitterItem->appendRow(childItem);

    // Fluctuation
    auto* fluxItem = new QStandardItem("Fluctuation");
    fluxItem->setEditable(false);
    fluxItem->setData(static_cast<s32>(NodeType::Fluctuation), sRoleNodeType);
    fluxItem->setData(setIndex, sRoleSetIdx);
    fluxItem->setData(emitterIndex, sRoleEmitterIdx);
    emitterItem->appendRow(fluxItem);

    // Field
    auto* fieldItem = new QStandardItem("Field");
    fieldItem->setEditable(false);
    fieldItem->setData(static_cast<s32>(NodeType::Field), sRoleNodeType);
    fieldItem->setData(setIndex, sRoleSetIdx);
    fieldItem->setData(emitterIndex, sRoleEmitterIdx);
    emitterItem->appendRow(fieldItem);
}

void PtclList::updateEmitter(s32 setIndex, s32 emitterIndex) {
    const QStandardItem* setItem = mListModel.item(setIndex);
    QStandardItem* emitterItem = setItem->child(emitterIndex);

    const auto& sets = mResPtr->getEmitterSets();
    const auto& set = sets[setIndex];
    const auto* emitter = set->emitters()[emitterIndex].get();

    if (emitter->type() == Ptcl::EmitterType::Simple) {
        // Remove Child, Flux and Field rows
        emitterItem->removeRows(0, 3);
    } else if (!emitterItem->hasChildren()){
        addComplexNodes(emitterItem, setIndex, emitterIndex);
    }
}

void PtclList::updateEmitterName(s32 setIndex, s32 emitterIndex) {
    const QStandardItem* setItem = mListModel.item(setIndex);
    QStandardItem* emitterItem = setItem->child(emitterIndex);

    const auto& sets = mResPtr->getEmitterSets();
    const auto& set = sets[setIndex];
    const auto* emitter = set->emitters()[emitterIndex].get();

    QString emitterName = QString("Emitter %1: %2").arg(emitterIndex).arg(emitter->name());
    emitterItem->setText(emitterName);
}

void PtclList::updateEmitterSetName(s32 setIndex) {
    QStandardItem* setItem = mListModel.item(setIndex);

    const auto& sets = mResPtr->getEmitterSets();
    const auto& set = sets[setIndex];

    QString setName = QString("%1: %2").arg(setIndex).arg(set->name());
    setItem->setText(setName);
}


// ========================================================================== //


} // namespace PtclEditor
