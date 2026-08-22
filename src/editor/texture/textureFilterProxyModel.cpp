#include "editor/texture/textureFilterProxyModel.h"
#include "editor/texture/textureListRoles.h"

#include <algorithm>


namespace PtclEditor {


// ========================================================================== //


TextureFilterProxyModel::TextureFilterProxyModel(QObject* parent) :
    QSortFilterProxyModel{parent} {}

void TextureFilterProxyModel::setEnabledFormats(const QSet<Ptcl::TextureFormat>& formats) {
    if (mEnabledFormats == formats) {
        return;
    }

    beginFilterChange();
    mEnabledFormats = formats;
    endFilterChange();
}

void TextureFilterProxyModel::setShowUnusedOnly(bool show) {
    if (mShowUnusedOnly == show) {
        return;
    }

    beginFilterChange();
    mShowUnusedOnly = show;
    endFilterChange();
}

void TextureFilterProxyModel::setMaxSize(s32 maxSize) {
    if (mMaxSize == maxSize) {
        return;
    }

    beginFilterChange();
    mMaxSize = maxSize;
    endFilterChange();
}

void TextureFilterProxyModel::setMaxFileSize(s64 maxFileSize) {
    if (mMaxFileSize == maxFileSize) {
        return;
    }

    beginFilterChange();
    mMaxFileSize = maxFileSize;
    endFilterChange();
}

void TextureFilterProxyModel::refreshFilter() {
    beginFilterChange();
    endFilterChange();
}

bool TextureFilterProxyModel::filterAcceptsRow(s32 sourceRow, const QModelIndex& sourceParent) const {
    if (sourceParent.isValid()) {
        return true;
    }

    const QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (!index.isValid()) {
        return false;
    }

    const auto format = static_cast<Ptcl::TextureFormat>(
        sourceModel()->data(index, TextureListRoles::FormatRole).toInt());
    if (!mEnabledFormats.isEmpty() && !mEnabledFormats.contains(format)) {
        return false;
    }

    if (mShowUnusedOnly && sourceModel()->data(index, TextureListRoles::UserCountRole).toUInt() != 0) {
        return false;
    }

    if (mMaxSize > 0) {
        const s32 width = sourceModel()->data(index, TextureListRoles::WidthRole).toInt();
        const s32 height = sourceModel()->data(index, TextureListRoles::HeightRole).toInt();
        if (std::max(width, height) > mMaxSize) {
            return false;
        }
    }

    if (mMaxFileSize > 0 &&
        sourceModel()->data(index, TextureListRoles::SizeRole).toLongLong() > mMaxFileSize) {
        return false;
    }

    return true;
}

bool TextureFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const {
    const auto* source = sourceModel();
    const s32 rowLeft = left.row();
    const s32 rowRight = right.row();

    auto intData = [&](s32 role) {
        return std::pair{
            source->data(left, role).toInt(),
            source->data(right, role).toInt()
        };
    };

    switch (sortColumn()) {
        case TextureColumn::ThumbnailColumn: {
            return rowLeft < rowRight;
        }
        case TextureColumn::FormatColumn: {
            const auto fmt = intData(TextureListRoles::FormatRole);
            if (fmt.first != fmt.second) {
                return fmt.first < fmt.second;
            }
            break;
        }
        case TextureColumn::DimensionsColumn: {
            const auto width = intData(TextureListRoles::WidthRole);
            const auto height = intData(TextureListRoles::HeightRole);

            const s32 maxLeft = std::max(width.first, height.first);
            const s32 maxRight = std::max(width.second, height.second);

            const s64 areaLeft = static_cast<s64>(width.first) * height.first;
            const s64 areaRight = static_cast<s64>(width.second) * height.second;

            return std::tie(maxLeft, areaLeft) < std::tie(maxRight, areaRight);
        }
        case TextureColumn::SizeColumn: {
            const auto size = intData(TextureListRoles::SizeRole);
            if (size.first != size.second) {
                return size.first < size.second;
            }
            break;
        }
        case TextureColumn::UsersColumn: {
            const auto users = intData(TextureListRoles::UserCountRole);
            if (users.first != users.second) {
                return users.first < users.second;
            }
            break;
        }
        default:
            break;
    }

    return rowLeft < rowRight;
}


// ========================================================================== //


} // namespace PtclEditor
