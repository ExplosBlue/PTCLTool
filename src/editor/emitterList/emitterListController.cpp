#include "editor/emitterList/emitterListController.h"

#include <QMessageBox>

#include <algorithm>


namespace PtclTool {


// ========================================================================== //


EmitterListController::EmitterListController(QObject* parent) :
    QObject{parent} {}

void EmitterListController::setDocument(Ptcl::Document* document) {
    if (mDocument) {
        mDocument->disconnect(this);
    }

    mDocument = document;

    if (!mDocument) {
        mListModel.clear();
        mClipboardSet.reset();
        mClipboardEmitter.reset();
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

    connect(mDocument, &Ptcl::Document::emitterSetAdded, this, [this](s32 setIndex) {
        insertEmitterSetNode(setIndex);
        reindexEmitterSets();
    });

    connect(mDocument, &Ptcl::Document::emitterSetRemoved, this, [this](s32 setIndex) {
        mListModel.removeRow(setIndex);
        reindexEmitterSets();
    });

    connect(mDocument, &Ptcl::Document::emitterChanged, this, [this](s32 setIndex, s32 emitterIndex) {
        updateEmitterName(setIndex, emitterIndex);
        updateEmitter(setIndex, emitterIndex);
    });

    connect(mDocument, &Ptcl::Document::emitterSetChanged, this, [this](s32 setIndex) {
        updateEmitterSetName(setIndex);
    });

    mListModel.clear();
    populate();
}

void EmitterListController::setSelection(Ptcl::Selection* selection) {
    mSelection = selection;
}

QStandardItemModel* EmitterListController::model() {
    return &mListModel;
}

const QStandardItemModel* EmitterListController::model() const {
    return &mListModel;
}

QString EmitterListController::itemLabel(s32 index, const QString& name) {
    return QString("%1: %2").arg(index).arg(name);
}

QStandardItem* EmitterListController::makeNode(const QString& label, NodeType type, s32 setIndex, s32 emitterIndex) {
    auto* item = new QStandardItem(label);
    item->setEditable(false);
    item->setData(static_cast<s32>(type), sRoleNodeType);
    item->setData(setIndex, sRoleSetIdx);
    if (emitterIndex >= 0) {
        item->setData(emitterIndex, sRoleEmitterIdx);
    }
    return item;
}

void EmitterListController::populate() {
    if (!mDocument) {
        return;
    }

    mListModel.clear();
    const auto& sets = mDocument->emitterSets();
    for (size_t setIndex = 0; setIndex < sets.size(); ++setIndex) {
        insertEmitterSetNode(static_cast<s32>(setIndex));
    }
}

QStandardItem* EmitterListController::findItem(s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type type) const {
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

bool EmitterListController::canPaste() const {
    return mClipboardSet || mClipboardEmitter;
}

void EmitterListController::insertEmitterSetNode(s32 setIndex) {
    const auto* set = mDocument->emitterSet(setIndex);
    if (!set) {
        return;
    }

    auto* setItem = makeNode(itemLabel(setIndex, set->name()), NodeType::EmitterSet, setIndex);

    for (s32 emitterIndex = 0; emitterIndex < mDocument->emitterCount(setIndex); ++emitterIndex) {
        insertEmitterNode(setItem, setIndex, emitterIndex);
    }
    mListModel.insertRow(setIndex, setItem);
}

void EmitterListController::insertEmitterNode(QStandardItem* setItem, s32 setIndex, s32 emitterIndex) {
    const auto* emitter = mDocument->emitter(setIndex, emitterIndex);
    if (!emitter) {
        return;
    }

    auto* emitterItem = makeNode(itemLabel(emitterIndex, emitter->name()), NodeType::Emitter, setIndex, emitterIndex);
    emitterItem->setData(static_cast<u32>(emitter->type()), sRoleEmitterType);

    if (emitter->type() == Ptcl::EmitterType::Complex || emitter->type() == Ptcl::EmitterType::Compact) {
        addComplexNodes(emitterItem, setIndex, emitterIndex);
    }
    setItem->insertRow(emitterIndex, emitterItem);
}

QStandardItem* EmitterListController::findChildByType(QStandardItem* parent, NodeType type) {
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

void EmitterListController::addComplexNodes(QStandardItem* emitterItem, s32 setIndex, s32 emitterIndex) {
    const auto& emitter = mDocument->emitter(setIndex, emitterIndex);

    ensureComplexNode(
        emitterItem,
        NodeType::ChildData,
        "ChildData",
        setIndex,
        emitterIndex,
        emitter->isChildEnabled()
    );

    ensureComplexNode(
        emitterItem,
        NodeType::Fluctuation,
        "Fluctuation",
        setIndex,
        emitterIndex,
        emitter->isFluctuationEnabled()
    );

    ensureComplexNode(
        emitterItem,
        NodeType::Field,
        "Field",
        setIndex,
        emitterIndex,
        emitter->isFieldEnabled()
    );
}

void EmitterListController::ensureComplexNode(QStandardItem* emitterItem, NodeType type, const QString& label, s32 setIndex, s32 emitterIndex, bool enabled) {
    auto* item = findChildByType(emitterItem, type);

    if (!item) {
        item = makeNode(label, type, setIndex, emitterIndex);
        emitterItem->appendRow(item);
    }

    item->setData(enabled, sRoleEnabled);
}

void EmitterListController::selectNearestValidEmitter(s32 setIndex, s32 preferredEmitter) {
    if (!mDocument || !mSelection) {
        return;
    }

    const auto& set = mDocument->emitterSet(setIndex);
    const s32 count = set ? set->emitterCount() : 0;

    if (count <= 0) {
        mSelection->set(setIndex, 0, Ptcl::Selection::Type::EmitterSet);
        return;
    }

    const s32 clamped = std::clamp(preferredEmitter, 0, count - 1);
    mSelection->set(setIndex, clamped, Ptcl::Selection::Type::Emitter);
}

void EmitterListController::selectNearestValidEmitterSet(s32 preferredSet) {
    if (!mDocument || !mSelection) {
        return;
    }

    const s32 count = mDocument->emitterSetCount();

    if (count <= 0) {
        return;
    }

    const s32 clamped = std::clamp(preferredSet, 0, count - 1);
    mSelection->set(clamped, 0, Ptcl::Selection::Type::EmitterSet);
}

void EmitterListController::reindexEmitters(QStandardItem* setItem, s32 setIndex) {
    if (!setItem) {
        return;
    }

    for (s32 i = 0; i < setItem->rowCount(); ++i) {
        QStandardItem* emitterItem = setItem->child(i);
        if (!emitterItem) {
            continue;
        }

        emitterItem->setData(i, sRoleEmitterIdx);
        const auto* emitter = mDocument->emitter(setIndex, i);
        if (!emitter) {
            continue;
        }
        emitterItem->setText(itemLabel(i, emitter->name()));

        for (s32 c = 0; c < emitterItem->rowCount(); ++c) {
            QStandardItem* child = emitterItem->child(c);
            if (child) {
                child->setData(i, sRoleEmitterIdx);
            }
        }
    }
}

void EmitterListController::reindexEmitterSets() {
    for (s32 i = 0; i < mListModel.rowCount(); ++i) {
        QStandardItem* setItem = mListModel.item(i);
        if (!setItem) {
            continue;
        }

        setItem->setData(i, sRoleSetIdx);
        const auto* set = mDocument->emitterSet(i);
        if (!set) {
            continue;
        }
        setItem->setText(itemLabel(i, set->name()));
        reindexEmitters(setItem, i);
    }
}

void EmitterListController::updateEmitter(s32 setIndex, s32 emitterIndex) {
    const QStandardItem* setItem = mListModel.item(setIndex);
    if (!setItem) {
        return;
    }

    QStandardItem* emitterItem = setItem->child(emitterIndex);
    if (!emitterItem) {
        return;
    }

    const auto* emitter = mDocument->emitter(setIndex, emitterIndex);
    if (!emitter) {
        return;
    }
    emitterItem->setData(static_cast<u32>(emitter->type()), sRoleEmitterType);

    if (emitter->type() == Ptcl::EmitterType::Simple) {
        emitterItem->removeRows(0, emitterItem->rowCount());
        return;
    }

    addComplexNodes(emitterItem, setIndex, emitterIndex);
}

void EmitterListController::updateEmitterName(s32 setIndex, s32 emitterIndex) {
    const QStandardItem* setItem = mListModel.item(setIndex);
    if (!setItem) {
        return;
    }

    QStandardItem* emitterItem = setItem->child(emitterIndex);
    if (!emitterItem) {
        return;
    }

    const auto* emitter = mDocument->emitter(setIndex, emitterIndex);
    if (!emitter) {
        return;
    }

    emitterItem->setText(itemLabel(emitterIndex, emitter->name()));
}

void EmitterListController::updateEmitterSetName(s32 setIndex) {
    QStandardItem* setItem = mListModel.item(setIndex);
    if (!setItem) {
        return;
    }

    const auto* set = mDocument->emitterSet(setIndex);
    if (!set) {
        return;
    }

    setItem->setText(itemLabel(setIndex, set->name()));
}

void EmitterListController::addEmitterSet(QStandardItem* contextItem) {
    if (!contextItem || !mDocument || !mSelection) {
        return;
    }

    const s32 setIndex = mDocument->emitterSetCount();
    mDocument->addEmitterSet("Add New EmitterSet");

    mSelection->set(setIndex, 0, Ptcl::Selection::Type::EmitterSet);
    emit contentChanged();
}

void EmitterListController::addEmitter(QStandardItem* contextItem) {
    if (!contextItem || !mDocument || !mSelection) {
        return;
    }

    const ListNodeRef ref = resolveListNodeRef(contextItem);
    const s32 setIndex = ref.setIndex;
    const auto& emitterSet = mDocument->emitterSet(setIndex);
    const s32 emitterIndex = emitterSet->emitterCount();

    mDocument->addEmitter("Add New Emitter", setIndex);
    mSelection->set(setIndex, emitterIndex, Ptcl::Selection::Type::Emitter);
    emit contentChanged();
}

void EmitterListController::removeItem(QStandardItem* contextItem) {
    const ListNodeRef ref = resolveListNodeRef(contextItem);

    switch (ref.type) {
    case NodeType::Emitter:
        removeEmitter(ref.setItem, ref.item);
        break;
    case NodeType::EmitterSet:
        removeEmitterSet(ref.item);
        break;
    default:
        break;
    }
}

void EmitterListController::removeEmitter(QStandardItem* setItem, QStandardItem* emitterItem) {
    if (!setItem || !mDocument || !mSelection) {
        return;
    }

    const ListNodeRef ref = resolveListNodeRef(emitterItem);
    const s32 setIndex = ref.setIndex;
    const s32 emitterIndex = ref.emitterIndex;

    const auto& emitter = mDocument->emitter(setIndex, emitterIndex);

    const auto confirmationMessage = QString("Are you sure you want to remove the Emitter '%1'?").arg(emitter->name());
    if (QMessageBox::question(nullptr, "Remove Emitter", confirmationMessage) != QMessageBox::Yes) {
        return;
    }

    const s32 nextPreferred = emitterIndex;
    mDocument->removeEmitter(setIndex, emitterIndex);
    selectNearestValidEmitter(setIndex, nextPreferred);
    emit contentChanged();
}

void EmitterListController::removeEmitterSet(QStandardItem* setItem) {
    if (!setItem || !mDocument || !mSelection) {
        return;
    }

    const s32 setIndex = setItem->data(sRoleSetIdx).toInt();
    const auto& emitterSet = mDocument->emitterSet(setIndex);
    const auto confirmationMessage = QString("Are you sure you want to remove the EmitterSet '%1'?").arg(emitterSet->name());
    if (QMessageBox::question(nullptr, "Remove EmitterSet", confirmationMessage) != QMessageBox::Yes) {
        return;
    }

    const s32 nextPreferred = setIndex;
    mDocument->removeEmitterSet(setIndex);
    selectNearestValidEmitterSet(nextPreferred);
    emit contentChanged();
}

void EmitterListController::copyItem(QStandardItem* contextItem) {
    if (!contextItem || !mDocument) {
        return;
    }

    const ListNodeRef ref = resolveListNodeRef(contextItem);
    mClipboardSet.reset();
    mClipboardEmitter.reset();

    switch (ref.type) {
    case NodeType::EmitterSet:
        mClipboardSet = mDocument->emitterSet(ref.setIndex)->clone();
        break;
    case NodeType::Emitter:
        mClipboardEmitter = mDocument->emitter(ref.setIndex, ref.emitterIndex)->clone();
        break;
    default:
        break;
    }

    emit contentChanged();
}

void EmitterListController::pasteItem(QStandardItem* contextItem) {
    if (!contextItem || !mDocument || !mSelection) {
        return;
    }

    if (mClipboardSet) {
        const s32 setIndex = mDocument->emitterSetCount();
        mDocument->addEmitterSet("Paste EmitterSet", mClipboardSet->clone());

        mSelection->set(setIndex, 0, Ptcl::Selection::Type::EmitterSet);
    } else if (mClipboardEmitter) {
        const ListNodeRef ref = resolveListNodeRef(contextItem);
        const s32 setIndex = ref.setIndex;

        auto set = mDocument->emitterSet(setIndex);
        const s32 emitterIndex = set->emitterCount();
        mDocument->addEmitter("Paste Emitter", setIndex, mClipboardEmitter->clone());

        mSelection->set(setIndex, emitterIndex, Ptcl::Selection::Type::Emitter);
    }

    emit contentChanged();
}

void EmitterListController::duplicateItem(QStandardItem* contextItem) {
    const ListNodeRef ref = resolveListNodeRef(contextItem);

    switch (ref.type) {
    case NodeType::EmitterSet:
        duplicateEmitterSet(ref.item);
        break;
    case NodeType::Emitter:
        duplicateEmitter(ref.item);
        break;
    default:
        break;
    }
}

void EmitterListController::duplicateEmitterSet(QStandardItem* contextItem) {
    if (!contextItem || !mDocument || !mSelection) {
        return;
    }

    const ListNodeRef ref = resolveListNodeRef(contextItem);
    if (!ref.isEmitterSet()) {
        return;
    }

    const s32 setIndex = ref.setIndex;
    const s32 newSetIndex = mDocument->emitterSetCount();
    mDocument->addEmitterSet("Duplicate EmitterSet", mDocument->emitterSet(setIndex)->clone());

    mSelection->set(newSetIndex, 0, Ptcl::Selection::Type::EmitterSet);
    emit contentChanged();
}

void EmitterListController::duplicateEmitter(QStandardItem* contextItem) {
    if (!contextItem || !mDocument || !mSelection) {
        return;
    }

    const ListNodeRef ref = resolveListNodeRef(contextItem);
    if (!ref.isEmitter()) {
        return;
    }

    const s32 setIndex = ref.setIndex;
    const s32 emitterIndex = ref.emitterIndex;
    auto set = mDocument->emitterSet(setIndex);

    const s32 newEmitterIndex = set->emitterCount();
    mDocument->addEmitter("Duplicate Emitter", setIndex, mDocument->emitter(setIndex, emitterIndex)->clone());

    mSelection->set(setIndex, newEmitterIndex, Ptcl::Selection::Type::Emitter);
    emit contentChanged();
}

bool EmitterListController::importEmitterSet(const QString& filePath) {
    if (!mDocument || filePath.isEmpty()) {
        return false;
    }

    if (!mDocument->importEmitterSet(filePath)) {
        return false;
    }

    emit contentChanged();
    return true;
}

bool EmitterListController::importEmitter(s32 setIndex, const QString& filePath) {
    if (!mDocument || filePath.isEmpty()) {
        return false;
    }

    if (!mDocument->importEmitter(setIndex, filePath)) {
        return false;
    }

    emit contentChanged();
    return true;
}

bool EmitterListController::exportEmitter(s32 setIndex, s32 emitterIndex, const QString& filePath) {
    if (!mDocument || filePath.isEmpty()) {
        return false;
    }

    return mDocument->exportEmitter(setIndex, emitterIndex, filePath);
}

bool EmitterListController::exportEmitterSet(s32 setIndex, const QString& filePath) {
    if (!mDocument || filePath.isEmpty()) {
        return false;
    }

    return mDocument->exportEmitterSet(setIndex, filePath);
}


// ========================================================================== //


} // namespace PtclTool
