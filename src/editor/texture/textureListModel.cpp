#include "editor/texture/textureListModel.h"
#include "editor/texture/textureListRoles.h"

#include "util/stringUtil.h"

#include <QPixmap>


namespace PtclEditor {


// ========================================================================== //


TextureListModel::TextureListModel(QObject* parent) :
    QAbstractTableModel{parent} {}

void TextureListModel::setTextures(const Ptcl::TextureList* textures) {
    beginResetModel();

    mTextures = textures;

    if (mTextures) {
        for (auto & tex : *mTextures) {
            tex->setUserCountCallback([this, texture = tex.get()]() {
                emitRowChangedFor(texture);
            });
        }
    }

    endResetModel();
}

s32 TextureListModel::rowCount(const QModelIndex& index) const {
    Q_UNUSED(index);
    return mTextures ? static_cast<s32>(mTextures->size()) : 0;
}

s32 TextureListModel::columnCount(const QModelIndex& index) const {
    Q_UNUSED(index);
    return TextureColumnCount;
}

QVariant TextureListModel::data(const QModelIndex& index, s32 role) const {
    if (!mTextures || !index.isValid()) {
        return {};
    }

    const auto& texture = (*mTextures)[index.row()];
    const auto& img = texture->textureData();

    switch (role) {
        case Qt::DisplayRole:
            switch (index.column()) {
                case TextureColumn::ThumbnailColumn:
                    return {};
                case TextureColumn::FormatColumn:
                    return toString(texture->textureFormat());
                case TextureColumn::DimensionsColumn:
                    return QString("%1 x %2").arg(img.width()).arg(img.height());
                case TextureColumn::SizeColumn:
                    return StringUtil::formatBytes(texture->textureDataRaw().size());
                case TextureColumn::UsersColumn:
                    return QString::number(texture->userCount());
                default:
                    return {};
            }
        case Qt::DecorationRole:
            if (index.column() == TextureColumn::ThumbnailColumn) {
                return QPixmap::fromImage(img);
            }
            return {};
        case TextureListRoles::TexturePtrRole:
            return QVariant::fromValue<void*>(texture.get());
        case TextureListRoles::IndexRole:
            return index.row();
        case TextureListRoles::FormatRole:
            return static_cast<s32>(texture->textureFormat());
        case TextureListRoles::SizeRole:
            return static_cast<s32>(texture->textureDataRaw().size());
        case TextureListRoles::UserCountRole:
            return texture->userCount();
        case TextureListRoles::WidthRole:
            return img.width();
        case TextureListRoles::HeightRole:
            return img.height();
        default:
            return {};
    }
}

QVariant TextureListModel::headerData(s32 section, Qt::Orientation orientation, s32 role) const {
    if (role != Qt::DisplayRole) {
        return {};
    }

    if (orientation == Qt::Vertical) {
        return section;
    }

    switch (section) {
        case TextureColumn::ThumbnailColumn:  return "Texture";
        case TextureColumn::FormatColumn:     return "Format";
        case TextureColumn::DimensionsColumn: return "Dimensions";
        case TextureColumn::SizeColumn:       return "Size";
        case TextureColumn::UsersColumn:      return "Users";
        default: return {};
    }
}

void TextureListModel::emitRowChangedFor(Ptcl::Texture* texture) {
    if (!mTextures || !texture) {
        return;
    }

    const s32 rowCount = static_cast<s32>(mTextures->size());
    for (s32 row = 0; row < rowCount; ++row) {
        if ((*mTextures)[row].get() == texture) {
            const QModelIndex topLeft = index(row, TextureColumn::ThumbnailColumn);
            const QModelIndex bottomRight = index(row, TextureColumn::TextureColumnCount - 1);
            emit dataChanged(topLeft, bottomRight, {Qt::DisplayRole, Qt::DecorationRole, TextureListRoles::UserCountRole});
            return;
        }
    }
}

void TextureListModel::onTextureAdded(s32 index) {
    if (!mTextures) {
        return;
    }

    beginInsertRows(QModelIndex(), index, index);

    auto* texture = (*mTextures)[index].get();
    texture->setUserCountCallback([this, texture]() {
        emitRowChangedFor(texture);
    });

    endInsertRows();
}

void TextureListModel::onTextureRemoved(s32 index) {
    if (!mTextures) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}


// ========================================================================== //


} // namespace PtclEditor
