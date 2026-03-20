#include "editor/ptclListWidget.h"

#include <QApplication>
#include <QMessageBox>


namespace PtclEditor {


// ========================================================================== //


EmitterFilterProxyModel::EmitterFilterProxyModel(QObject* parent) :
    QSortFilterProxyModel{parent} {}

void EmitterFilterProxyModel::setEmitterFilter(const EmitterFilter& filter) {
    if (mEmitterFilter == filter) {
        return;
    }

    beginFilterChange();
    mEmitterFilter = filter;
    endFilterChange();
}

bool EmitterFilterProxyModel::filterAcceptsRow(s32 sourceRow, const QModelIndex& sourceParent) const {
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (!index.isValid()) {
        return false;
    }

    // Type Filter
    const auto nodeType = static_cast<NodeType>(sourceModel()->data(index, sRoleNodeType).toUInt());

    if (nodeType == NodeType::Emitter) {
        const auto emitterType = static_cast<Ptcl::EmitterType>(sourceModel()->data(index, sRoleEmitterType).toUInt());
        bool allowed = false;

        switch (emitterType) {
        case Ptcl::EmitterType::Simple:
            allowed = mEmitterFilter.isSet(EmitterFilterFlag::Simple);
            break;
        case Ptcl::EmitterType::Complex:
            allowed = mEmitterFilter.isSet(EmitterFilterFlag::Complex);
            break;
        case Ptcl::EmitterType::Compact:
            allowed = mEmitterFilter.isSet(EmitterFilterFlag::Compact);
            break;
        default:
            break;
        }

        if (!allowed) {
            return false;
        }
    }

    // Search filter
    const auto re = filterRegularExpression();

    if (sourceModel()->data(index).toString().contains(re)) {
        return true;
    }

    for (QModelIndex parent = sourceParent; parent.isValid(); parent = parent.parent()) {
        if (sourceModel()->data(parent).toString().contains(re)) {
            return true;
        }
    }

    const s32 childCount = sourceModel()->rowCount(index);
    for (s32 i = 0; i < childCount; ++i) {
        if (filterAcceptsRow(i, index)) {
            return true;
        }
    }

    return false;
}

QVariant EmitterFilterProxyModel::data(const QModelIndex& index, s32 role) const {
    if (!index.isValid()) {
        return {};
    }

    const auto srcIndex = mapToSource(index);
    if (!srcIndex.isValid()) {
        return QSortFilterProxyModel::data(index, role);
    }

    const auto type = static_cast<NodeType>(sourceModel()->data(srcIndex, sRoleNodeType).toUInt());

    if (role == Qt::FontRole) {
        if (type == NodeType::EmitterSet) {
            auto font = QSortFilterProxyModel::data(index, role).value<QFont>();
            font.setBold(true);
            return font;
        }
    }

    const bool isComplexNode = (
        type == NodeType::ChildData ||
        type == NodeType::Fluctuation ||
        type == NodeType::Field
    );

    if (isComplexNode && role == Qt::ForegroundRole) {
        const bool enabled = index.data(sRoleEnabled).toBool();
        if (!enabled) {
            auto color = QSortFilterProxyModel::data(index, role).value<QColor>();
            if (!color.isValid()) {
                color = QApplication::palette().color(QPalette::Disabled, QPalette::Text);
            }
            return color;
        }
    }
    return QSortFilterProxyModel::data(index, role);
}


// ========================================================================== //


PtclList::PtclList(QWidget* parent) :
    QWidget{parent} {
    // Toolbar
    mToolBar.setIconSize(QSize(24, 24));
    mToolBar.setToolButtonStyle(Qt::ToolButtonIconOnly);

    mAddEmitterSetAction = mToolBar.addAction(QIcon(":/res/icons/add_emitterset.png"), "Add Emitter Set");
    connect(mAddEmitterSetAction, &QAction::triggered, this, &PtclList::addEmitterSet);

    mAddEmitterAction = mToolBar.addAction(QIcon(":/res/icons/add_emitter.png"), "Add Emitter");
    connect(mAddEmitterAction, &QAction::triggered, this, &PtclList::addEmitter);

    mToolBar.addSeparator();

    mRemoveAction = mToolBar.addAction(QIcon(":/res/icons/remove.png"), "Remove");
    connect(mRemoveAction, &QAction::triggered, this, &PtclList::removeItem);

    mToolBar.addSeparator();

    mCopyAction = mToolBar.addAction(QIcon(":/res/icons/copy.png"), "Copy");
    connect(mCopyAction, &QAction::triggered, this, &PtclList::copyItem);

    mPasteAction = mToolBar.addAction(QIcon(":/res/icons/paste.png"), "Paste");
    connect(mPasteAction, &QAction::triggered, this, &PtclList::pasteItem);

    for (auto* act : { mAddEmitterSetAction, mAddEmitterAction, mRemoveAction, mCopyAction, mPasteAction }) {
        act->setEnabled(false);
    }

    // Search Box
    mSearchBox.setPlaceholderText("Search");
    connect(&mSearchBox, &QLineEdit::textChanged, this, &PtclList::filterList);
    connect(&mSearchBox, &QLineEdit::textChanged, this, [this] {
        mTreeView.expandAll();
    });

    // Filter Button
    mFilterButton.setIcon(QIcon(":/res/icons/filter.png"));
    mFilterButton.setPopupMode(QToolButton::InstantPopup);
    mFilterButton.setMenu(&mFilterMenu);

    setupFilterMenu();

    // Proxy Model
    mProxyModel.setSourceModel(&mListModel);
    mProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    mProxyModel.setRecursiveFilteringEnabled(false);

    // Tree View
    mTreeView.setModel(&mProxyModel);
    mTreeView.setHeaderHidden(true);
    connect(mTreeView.selectionModel(), &QItemSelectionModel::selectionChanged, this, &PtclList::selectionChanged);

    // Search Layout
    auto* searchLayout = new QHBoxLayout;
    searchLayout->addWidget(&mSearchBox);
    searchLayout->addWidget(&mFilterButton);

    // Main layout
    mMainLayout.addWidget(&mToolBar);
    mMainLayout.addWidget(&mTreeView);
    mMainLayout.addLayout(searchLayout);

    setLayout(&mMainLayout);
}

void PtclList::setupFilterMenu() {
    auto* simpleAction = mFilterMenu.addAction("Simple Emitters");
    auto* complexAction = mFilterMenu.addAction("Complex Emitters");
    auto* compactAction = mFilterMenu.addAction("Compact Emitters");
    mFilterMenu.addSeparator();
    auto* allAction = mFilterMenu.addAction("Show All");

    for (auto* act : { simpleAction, complexAction, compactAction }) {
        act->setCheckable(true);
        act->setChecked(true);
    }

    connect(allAction, &QAction::triggered, this, [simpleAction, complexAction, compactAction, this] {
        simpleAction->setChecked(true);
        complexAction->setChecked(true);
        compactAction->setChecked(true);
        mProxyModel.setEmitterFilter({
            EmitterFilterFlag::Simple,
            EmitterFilterFlag::Complex,
            EmitterFilterFlag::Compact
        });
    });

    auto updateFilter = [simpleAction, complexAction, compactAction, this] {
        EmitterFilter filter{};
        if (simpleAction->isChecked()) { filter.enable(EmitterFilterFlag::Simple); }
        if (complexAction->isChecked()) { filter.enable(EmitterFilterFlag::Complex); }
        if (compactAction->isChecked()) { filter.enable(EmitterFilterFlag::Compact); }
        mProxyModel.setEmitterFilter(filter);
    };

    connect(simpleAction, &QAction::toggled, this, updateFilter);
    connect(complexAction, &QAction::toggled, this, updateFilter);
    connect(compactAction, &QAction::toggled, this, updateFilter);
}

void PtclList::setDocument(Ptcl::Document* document) {
    if (mDocument) {
        mDocument->disconnect(this);
    }

    mDocument = document;

    if (!mDocument) {
        mListModel.clear();
        setEnabled(false);
        return;
    }

    connect(mDocument, &Ptcl::Document::emitterAdded, this, [this](s32 setIndex, s32 emitterIndex) {
        QStandardItem* setItem = mListModel.item(setIndex);
        if (!setItem) {
            return;
        }

        insertEmitterNode(setItem, setIndex, emitterIndex);
        reindexEmitters(setItem, setIndex);
    });

    connect(mDocument, &Ptcl::Document::emitterRemoved, this, [this](s32 setIndex, s32 emitterIndex) {
        QStandardItem* setItem = mListModel.item(setIndex);
        if (!setItem) {
            return;
        }

        setItem->removeRow(emitterIndex);
        reindexEmitters(setItem, setIndex);
    });

    mListModel.clear();
    populateList();
    setEnabled(true);
}

void PtclList::setSelection(Ptcl::Selection* selection) {
    mSelection = selection;

    connect(selection, &Ptcl::Selection::selectionChanged, this, [this](s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type type) {
        if (!mDocument) {
            return;
        }

        QSignalBlocker b(mTreeView.selectionModel());

        const QStandardItem* item = findItem(setIndex, emitterIndex, type);
        if (!item) {
            mTreeView.clearSelection();
            return;
        }

        const QModelIndex sourceIndex = mListModel.indexFromItem(item);
        const QModelIndex proxyIndex = mProxyModel.mapFromSource(sourceIndex);

        if (!proxyIndex.isValid()) {
            mTreeView.clearSelection();
            return;
        }

        auto* selectionModel = mTreeView.selectionModel();
        selectionModel->clearSelection();
        selectionModel->setCurrentIndex(proxyIndex, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        mTreeView.scrollTo(proxyIndex);
    });
}

void PtclEditor::PtclList::refresh() {
    populateList();
}

void PtclList::populateList() {
    if (!mDocument) {
        return;
    }

    mListModel.clear();
    const auto& sets = mDocument->emitterSets();
    for (s32 setIndex = 0; setIndex < sets.size(); ++setIndex) {
        insertEmitterSetNode(setIndex);
    }

    mTreeView.expandAll();
    filterList(mSearchBox.text());
}

void PtclList::insertEmitterSetNode(s32 setIndex) {
    const auto& set = mDocument->emitterSet(setIndex);

    QString setName = QString("%1: %2").arg(setIndex).arg(set->name());
    auto* setItem = new QStandardItem(setName);
    setItem->setEditable(false);
    setItem->setData(static_cast<s32>(NodeType::EmitterSet), sRoleNodeType);
    setItem->setData(setIndex, sRoleSetIdx);
    setItem->setIcon(QIcon(":/res/icons/emitterset.png"));

    // Emitters
    for (s32 emitterIndex = 0; emitterIndex < set->emitters().size(); ++emitterIndex) {
        insertEmitterNode(setItem, setIndex, emitterIndex);
    }
    mListModel.appendRow(setItem);
}

void PtclList::insertEmitterNode(QStandardItem* setItem, s32 setIndex, s32 emitterIndex) {
    const auto& emitter = mDocument->emitter(setIndex, emitterIndex);

    QString emitterName = QString("%1: %2").arg(emitterIndex).arg(emitter->name());
    auto* emitterItem = new QStandardItem(emitterName);
    emitterItem->setEditable(false);
    emitterItem->setData(static_cast<s32>(NodeType::Emitter), sRoleNodeType);
    emitterItem->setData(setIndex, sRoleSetIdx);
    emitterItem->setData(emitterIndex, sRoleEmitterIdx);
    emitterItem->setData(static_cast<u32>(emitter->type()), sRoleEmitterType);
    emitterItem->setIcon(QIcon(":/res/icons/emitter.png"));

    // Complex Data
    if (emitter->type() == Ptcl::EmitterType::Complex || emitter->type() == Ptcl::EmitterType::Compact) {
        addComplexNodes(emitterItem, setIndex, emitterIndex);
    }
    setItem->appendRow(emitterItem);
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

    updateToolbarForSelection(item);

    if (!item) {
        return;
    }

    auto type = static_cast<NodeType>(item->data(sRoleNodeType).toUInt());

    switch (type) {
        case NodeType::EmitterSet: {
            const s32 setIndex = sourceIndex.row();
            mSelection->set(setIndex, 0, Ptcl::Selection::Type::EmitterSet);
            break;
        }
        case NodeType::Emitter: {
            const s32 emitterIndex = sourceIndex.row();
            const s32 setIndex = sourceIndex.parent().row();
            mSelection->set(setIndex, emitterIndex, Ptcl::Selection::Type::Emitter);
            break;
        }
        case NodeType::ChildData: {
            const s32 emitterIndex = sourceIndex.parent().row();
            const s32 setIndex = sourceIndex.parent().parent().row();
            mSelection->set(setIndex, emitterIndex, Ptcl::Selection::Type::EmitterChild);
            break;
        }
        case NodeType::Fluctuation: {
            const s32 emitterIndex = sourceIndex.parent().row();
            const s32 setIndex = sourceIndex.parent().parent().row();
            mSelection->set(setIndex, emitterIndex, Ptcl::Selection::Type::EmitterFlux);
            break;
        }
        case NodeType::Field: {
            const s32 emitterIndex = sourceIndex.parent().row();
            const s32 setIndex = sourceIndex.parent().parent().row();
            mSelection->set(setIndex, emitterIndex, Ptcl::Selection::Type::EmitterField);
            break;
        }
    }
}

void PtclList::addComplexNodes(QStandardItem* emitterItem, s32 setIndex, s32 emitterIndex) {
    const auto& emitter = mDocument->emitter(setIndex, emitterIndex);

    // ChildData
    ensureComplexNode(
        emitterItem,
        NodeType::ChildData,
        "ChildData",
        setIndex,
        emitterIndex,
        emitter->isChildEnabled()
    );

    // Fluctuation
    ensureComplexNode(
        emitterItem,
        NodeType::Fluctuation,
        "Fluctuation",
        setIndex,
        emitterIndex,
        emitter->isFluctuationEnabled()
    );

    // Field
    ensureComplexNode(
        emitterItem,
        NodeType::Field,
        "Field",
        setIndex,
        emitterIndex,
        emitter->isFieldEnabled()
    );
}

QStandardItem* PtclList::findItem(s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type type) const {
    auto* setItem = mListModel.item(setIndex);
    if (!setItem) {
        return nullptr;
    }

    if (type == Ptcl::Selection::Type::EmitterSet) {
        return setItem;
    }

    auto* emitterItem = setItem->child(emitterIndex);
    if (!emitterItem) {
        return nullptr;
    }

    switch (type) {
    case Ptcl::Selection::Type::Emitter:
        return emitterItem;
    case Ptcl::Selection::Type::EmitterChild:
        return findChildByType(emitterItem, NodeType::ChildData);
    case Ptcl::Selection::Type::EmitterFlux:
        return findChildByType(emitterItem, NodeType::Fluctuation);
    case Ptcl::Selection::Type::EmitterField:
        return findChildByType(emitterItem, NodeType::Field);
    default:
        return nullptr;
    }
}

QStandardItem* PtclList::findChildByType(QStandardItem* parent, NodeType type) {
    if (!parent) {
        return nullptr;
    }

    for (s32 i = 0; i < parent->rowCount(); ++i) {
        auto* child = parent->child(i);
        if (!child) {
            continue;
        }

        if (child->data(sRoleNodeType).toUInt() == static_cast<u32>(type)) {
            return child;
        }
    }
    return nullptr;
}

void PtclList::ensureComplexNode(QStandardItem* emitterItem, NodeType type, const QString& label, s32 setIndex, s32 emitterIndex, bool enabled) {
    auto* item = findChildByType(emitterItem, type);

    if (!item) {
        item = new QStandardItem(label);
        item->setEditable(false);
        item->setData(static_cast<s32>(type), sRoleNodeType);
        item->setData(setIndex, sRoleSetIdx);
        item->setData(emitterIndex, sRoleEmitterIdx);
        emitterItem->appendRow(item);
    }

    item->setData(enabled, sRoleEnabled);
}

void PtclList::updateEmitter(s32 setIndex, s32 emitterIndex) {
    const QStandardItem* setItem = mListModel.item(setIndex);
    if (!setItem) {
        return;
    }

    QStandardItem* emitterItem = setItem->child(emitterIndex);
    if (!emitterItem) {
        return;
    }

    const auto& emitter = mDocument->emitter(setIndex, emitterIndex);
    emitterItem->setData(static_cast<u32>(emitter->type()), sRoleEmitterType);

    if (emitter->type() == Ptcl::EmitterType::Simple) {
        emitterItem->removeRows(0, emitterItem->rowCount());
        return;
    }

    addComplexNodes(emitterItem, setIndex, emitterIndex);
}

void PtclList::updateEmitterName(s32 setIndex, s32 emitterIndex) {
    const QStandardItem* setItem = mListModel.item(setIndex);
    QStandardItem* emitterItem = setItem->child(emitterIndex);

    const auto& emitter = mDocument->emitter(setIndex, emitterIndex);

    QString emitterName = QString("%1: %2").arg(emitterIndex).arg(emitter->name());
    emitterItem->setText(emitterName);
}

void PtclList::updateEmitterSetName(s32 setIndex) {
    QStandardItem* setItem = mListModel.item(setIndex);

    const auto& set = mDocument->emitterSet(setIndex);

    QString setName = QString("%1: %2").arg(setIndex).arg(set->name());
    setItem->setText(setName);
}

void PtclList::updateToolbarForSelection(const QStandardItem* item) {
    mAddEmitterSetAction->setEnabled(true);
    mAddEmitterAction->setEnabled(false);
    mRemoveAction->setEnabled(false);
    mCopyAction->setEnabled(false);
    mPasteAction->setEnabled(false);

    if (!item) {
        return;
    }

    const auto type = static_cast<NodeType>(item->data(sRoleNodeType).toUInt());

    mPasteAction->setEnabled(mClipboardSet || mClipboardEmitter);

    switch (type) {
    case NodeType::EmitterSet: {
        mAddEmitterAction->setEnabled(true);

        const s32 emitterSetCount = mListModel.rowCount();
        mRemoveAction->setEnabled(emitterSetCount > 1);
        mCopyAction->setEnabled(true);
        break;
    }

    case NodeType::Emitter: {
        mAddEmitterAction->setEnabled(true);

        const auto* setItem = item->parent();
        if (!setItem) {
            break;
        }

        const s32 emitterCount = setItem->rowCount();
        mRemoveAction->setEnabled(emitterCount > 1);
        mCopyAction->setEnabled(true);
        break;
    }
    case NodeType::ChildData:
    case NodeType::Fluctuation:
    case NodeType::Field:
        mAddEmitterAction->setEnabled(false);
        break;
    }
}

void PtclList::addEmitterSet() {
    QModelIndex proxyIndex = mTreeView.currentIndex();
    QModelIndex sourceIndex = mProxyModel.mapToSource(proxyIndex);
    QStandardItem* item = mListModel.itemFromIndex(sourceIndex);

    if (!item) {
        return;
    }

    const s32 setIndex = mDocument->emitterSetCount();
    mDocument->data().addNewEmitterSet();

    insertEmitterSetNode(setIndex);
    mSelection->set(setIndex, 0, Ptcl::Selection::Type::EmitterSet);
    expandSourceIndex(mListModel.index(setIndex, 0));
    emit itemAdded();
}

void PtclList::addEmitter() {
    QModelIndex proxyIndex = mTreeView.currentIndex();
    QModelIndex sourceIndex = mProxyModel.mapToSource(proxyIndex);
    QStandardItem* item = mListModel.itemFromIndex(sourceIndex);

    if (!item) {
        return;
    }

    auto type = static_cast<NodeType>(item->data(sRoleNodeType).toUInt());
    QStandardItem* setItem = item;
    if (type == NodeType::Emitter) {
        setItem = item->parent();
    }

    s32 setIndex = setItem->data(sRoleSetIdx).toInt();
    const auto& emitterSet = mDocument->emitterSet(setIndex);
    const s32 emitterIndex = emitterSet->emitterCount();

    mDocument->addEmitter(setIndex);

    mSelection->set(setIndex, emitterIndex, Ptcl::Selection::Type::Emitter);
    expandSourceIndex(mListModel.indexFromItem(setItem));
    emit itemAdded();
}

void PtclList::removeItem() {
    QModelIndex proxyIndex = mTreeView.currentIndex();
    QModelIndex sourceIndex = mProxyModel.mapToSource(proxyIndex);
    QStandardItem* item = mListModel.itemFromIndex(sourceIndex);

    if (!item) {
        return;
    }

    auto type = static_cast<NodeType>(item->data(sRoleNodeType).toUInt());

    if (type == NodeType::Emitter) {
        removeEmitter(item->parent(), item);
    } else if (type == NodeType::EmitterSet) {
        removeEmitterSet(item);
    }
    emit itemRemoved();
}


void PtclList::removeEmitter(QStandardItem* setItem, QStandardItem* emitterItem) {
    if (!setItem) {
        return;
    }

    const s32 setIndex = setItem->data(sRoleSetIdx).toInt();
    const s32 emitterIndex = emitterItem->data(sRoleEmitterIdx).toInt();

    const auto& emitterSet = mDocument->emitterSet(setIndex);
    const auto& emitter = mDocument->emitter(setIndex, emitterIndex);

    const auto confirmationMessage = QString("Are you sure you want to remove the Emitter '%1'?").arg(emitter->name());
    if (QMessageBox::question(this, "Remove Emitter", confirmationMessage) != QMessageBox::Yes) {
        return;
    }

    mDocument->removeEmitter(setIndex, emitterIndex);

    setItem->removeRow(emitterIndex);
    reindexEmitters(setItem, setIndex);

    const s32 remainingCount = setItem->rowCount();
    if (remainingCount > 0) {
        const s32 nextIndex = std::min(emitterIndex, remainingCount - 1);
        mSelection->set(setIndex, nextIndex, Ptcl::Selection::Type::Emitter);
    }
}

void PtclList::removeEmitterSet(QStandardItem* setItem) {
    if (!setItem) {
        return;
    }

    const s32 setIndex = setItem->data(sRoleSetIdx).toInt();
    const auto& emitterSet = mDocument->emitterSet(setIndex);

    const auto confirmationMessage = QString("Are you sure you want to remove the EmitterSet '%1'?").arg(emitterSet->name());
    if (QMessageBox::question(this, "Remove EmitterSet", confirmationMessage) != QMessageBox::Yes) {
        return;
    }

    mDocument->data().removeEmitterSet(setIndex);
    mListModel.removeRow(setIndex);
    reindexEmitterSets();

    const s32 remainingCount = mListModel.rowCount();

    if (remainingCount > 0) {
        const s32 nextIndex = std::min(setIndex, remainingCount - 1);
        mSelection->set(nextIndex, 0, Ptcl::Selection::Type::EmitterSet);
    }
}

void PtclList::reindexEmitters(QStandardItem* setItem, s32 setIndex) {
    if (!setItem) {
        return;
    }

    for (s32 i = 0; i < setItem->rowCount(); ++i) {
        QStandardItem* emitterItem = setItem->child(i);
        if (!emitterItem) {
            continue;
        }

        emitterItem->setData(i, sRoleEmitterIdx);
        const auto& emitter = mDocument->emitter(setIndex, i);
        emitterItem->setText(QString("%1: %2").arg(i).arg(emitter->name()));

        for (s32 c = 0; c < emitterItem->rowCount(); ++c) {
            QStandardItem* child = emitterItem->child(c);
            if (child) {
                child->setData(i, sRoleEmitterIdx);
            }
        }
    }
}

void PtclList::reindexEmitterSets() {
    for (s32 i = 0; i < mListModel.rowCount(); ++i) {
        QStandardItem* setItem = mListModel.item(i);
        if (!setItem) {
            continue;
        }

        setItem->setData(i, sRoleSetIdx);
        const auto& set = mDocument->emitterSet(i);
        setItem->setText(QString("%1: %2").arg(i).arg(set->name()));
        reindexEmitters(setItem, i);
    }
}

void PtclList::expandSourceIndex(const QModelIndex& sourceIndex) {
    const QModelIndex proxyIndex = mProxyModel.mapFromSource(sourceIndex);
    if (proxyIndex.isValid()) {
        mTreeView.expand(proxyIndex);
    }
}

void PtclList::copyItem() {
    QModelIndex proxyModel = mTreeView.currentIndex();
    QModelIndex sourceIndex = mProxyModel.mapToSource(proxyModel);
    QStandardItem* item = mListModel.itemFromIndex(sourceIndex);

    if (!item) {
        return;
    }

    const auto type = static_cast<NodeType>(item->data(sRoleNodeType).toUInt());
    mClipboardSet.reset();
    mClipboardEmitter.reset();

    if (type == NodeType::EmitterSet) {
        const s32 setIndex = item->data(sRoleSetIdx).toInt();
        mClipboardSet = mDocument->emitterSet(setIndex)->clone();
    } else if (type == NodeType::Emitter) {
        const s32 setIndex = item->parent()->data(sRoleSetIdx).toInt();
        const s32 emitterIndex = item->data(sRoleEmitterIdx).toInt();
        mClipboardEmitter = mDocument->emitter(setIndex, emitterIndex)->clone();
    }

    updateToolbarForSelection(item);
}

void PtclList::pasteItem() {
    QModelIndex proxyModel = mTreeView.currentIndex();
    QModelIndex sourceIndex = mProxyModel.mapToSource(proxyModel);
    QStandardItem* item = mListModel.itemFromIndex(sourceIndex);

    if (!item) {
        return;
    }

    const auto type = static_cast<NodeType>(item->data(sRoleNodeType).toUInt());

    if (mClipboardSet && type == NodeType::EmitterSet) {
        auto& newSet = mDocument->data().appendEmitterSet(mClipboardSet);
        mClipboardSet = newSet->clone();

        const s32 setIndex = mDocument->emitterSetCount() - 1;
        insertEmitterSetNode(setIndex);
        mSelection->set(setIndex, 0, Ptcl::Selection::Type::EmitterSet);
    } else if (mClipboardEmitter && type == NodeType::Emitter) {
        const auto& setItem = item->parent();
        const s32 setIndex = item->data(sRoleSetIdx).toInt();
        auto set = mDocument->emitterSet(setIndex);

        auto& newEmitter = set->appendEmitter(mClipboardEmitter);
        mClipboardEmitter = newEmitter->clone();

        const s32 emitterIndex = set->emitterCount() - 1;
        insertEmitterNode(setItem, setIndex, emitterIndex);

        mSelection->set(setIndex, emitterIndex, Ptcl::Selection::Type::Emitter);
    }
}

// ========================================================================== //


} // namespace PtclEditor
