#include "editor/emitterList/emitterFilterProxyModel.h"
#include "ptcl/ptclEnum.h"

#include <QApplication>
#include <QFont>
#include <QPalette>


namespace PtclTool {


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


} // namespace PtclTool
