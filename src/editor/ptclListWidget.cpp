#include "editor/ptclListWidget.h"

#include <QApplication>


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

            QString emitterName = QString("%1: %2").arg(emitterIndex).arg(emitter->name());
            auto* emitterItem = new QStandardItem(emitterName);
            emitterItem->setEditable(false);
            emitterItem->setData(static_cast<s32>(NodeType::Emitter), sRoleNodeType);
            emitterItem->setData(setIndex, sRoleSetIdx);
            emitterItem->setData(emitterIndex, sRoleEmitterIdx);
            emitterItem->setData(static_cast<u32>(emitter->type()), sRoleEmitterType);

            // Complex Data
            if (emitter->type() == Ptcl::EmitterType::Complex || emitter->type() == Ptcl::EmitterType::Compact) {
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
    const auto& emitter = mResPtr->getEmitterSets()[setIndex]->emitters()[emitterIndex];
    const auto& props = emitter->complexProperties();

    // ChildData
    ensureComplexNode(
        emitterItem,
        NodeType::ChildData,
        "ChildData",
        setIndex,
        emitterIndex,
        props.childFlags.isSet(Ptcl::ChildFlag::Enabled)
    );

    // Fluctuation
    ensureComplexNode(
        emitterItem,
        NodeType::Fluctuation,
        "Fluctuation",
        setIndex,
        emitterIndex,
        props.fluctuationFlags.isSet(Ptcl::FluctuationFlag::Enabled)
    );

    // Field
    ensureComplexNode(
        emitterItem,
        NodeType::Field,
        "Field",
        setIndex,
        emitterIndex,
        props.fieldFlags.isSet(Ptcl::FieldFlag::Enabled)
    );
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

    const auto& emitter = mResPtr->getEmitterSets()[setIndex]->emitters()[emitterIndex];
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

    const auto& sets = mResPtr->getEmitterSets();
    const auto& set = sets[setIndex];
    const auto* emitter = set->emitters()[emitterIndex].get();

    QString emitterName = QString("%1: %2").arg(emitterIndex).arg(emitter->name());
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
