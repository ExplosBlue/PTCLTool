#pragma once

#include "util/bitflagUtil.h"

#include "typedefs.h"

#include <QStandardItem>


namespace PtclTool {


// ========================================================================== //


enum class NodeType {
    EmitterSet,
    Emitter,
    ChildData,
    Fluctuation,
    Field
};

enum class EmitterFilterFlag {
    Simple  = 1 << 0,
    Complex = 1 << 1,
    Compact = 1 << 2,
};

using EmitterFilter = BitFlag<EmitterFilterFlag>;

static constexpr s32 sRoleNodeType      = Qt::UserRole;
static constexpr s32 sRoleSetIdx        = Qt::UserRole + 1;
static constexpr s32 sRoleEmitterIdx    = Qt::UserRole + 2;
static constexpr s32 sRoleEnabled       = Qt::UserRole + 3;
static constexpr s32 sRoleEmitterType   = Qt::UserRole + 4;

struct ListNodeRef {
    QStandardItem* item{nullptr};
    NodeType type{NodeType::EmitterSet};
    s32 setIndex{-1};
    s32 emitterIndex{-1};
    QStandardItem* setItem{nullptr};

    bool isValid() const { return item != nullptr; }
    bool isEmitterSet() const { return type == NodeType::EmitterSet; }
    bool isEmitter() const { return type == NodeType::Emitter; }
};

inline ListNodeRef resolveListNodeRef(QStandardItem* item) {
    ListNodeRef ref;
    if (!item) {
        return ref;
    }

    ref.item = item;
    ref.type = static_cast<NodeType>(item->data(sRoleNodeType).toUInt());
    ref.setIndex = item->data(sRoleSetIdx).toInt();
    ref.emitterIndex = item->data(sRoleEmitterIdx).toInt();

    if (ref.isEmitter()) {
        ref.setItem = item->parent();
    } else {
        ref.setItem = item;
    }
    return ref;
}


// ========================================================================== //


} //namespace PtclTool
