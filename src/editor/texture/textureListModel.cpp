#include "editor/texture/textureListModel.h"
#include "editor/texture/textureListRoles.h"

#include "util/stringUtil.h"

#include <QPixmap>


namespace PtclEditor {


// ========================================================================== //


TextureListModel::TextureListModel(QObject* parent) :
    QAbstractListModel{parent} {}

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

QVariant TextureListModel::data(const QModelIndex& index, s32 role) const {
    if (!mTextures || !index.isValid()) {
        return {};
    }

    const auto& texture = (*mTextures)[index.row()];
    const auto& img = texture->textureData();

    switch (role) {
        case Qt::DisplayRole: {
            const QString sizeString = StringUtil::formatBytes(texture->textureDataRaw().size());

            return QString("Format: %1\nDimensions: %2x%3\nSize %4\nUsers: %5")
                .arg(toString(texture->textureFormat()))
                .arg(img.width())
                .arg(img.height())
                .arg(sizeString)
                .arg(texture->userCount());
        }
        case Qt::DecorationRole:
            return QPixmap::fromImage(texture->textureData());
        case TextureListRoles::TexturePtrRole:
            return QVariant::fromValue<void*>(texture.get());
        case TextureListRoles::FormatRole:
            return static_cast<s32>(texture->textureFormat());
        case TextureListRoles::SizeRole:
            return static_cast<qint64>(texture->textureDataRaw().size());
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

void TextureListModel::emitRowChangedFor(Ptcl::Texture* texture) {
    if (!mTextures || !texture) {
        return;
    }

    const s32 rowCount = static_cast<s32>(mTextures->size());
    for (s32 row = 0; row < rowCount; ++row) {
        if ((*mTextures)[row].get() == texture) {
            QModelIndex idx = index(row);
            emit dataChanged(idx, idx, {Qt::DisplayRole, UserCountRole});
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
