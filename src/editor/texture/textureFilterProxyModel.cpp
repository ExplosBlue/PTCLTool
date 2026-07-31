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

void TextureFilterProxyModel::setMaxFileSize(s32 maxFileSize) {
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
        sourceModel()->data(index, TextureListRoles::SizeRole).toInt() > mMaxFileSize) {
        return false;
    }

    return true;
}


// ========================================================================== //


} // namespace PtclEditor
