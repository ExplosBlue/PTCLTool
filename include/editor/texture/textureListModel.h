#pragma once

#include "ptcl/ptcl.h"

#include <QAbstractTableModel>


namespace PtclEditor {


// ========================================================================== //


class TextureListModel final : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit TextureListModel(QObject* parent = nullptr);

    void setTextures(const Ptcl::TextureList* textures);

    s32 rowCount(const QModelIndex& parent = {}) const final;
    s32 columnCount(const QModelIndex& parent = {}) const final;

    QVariant data(const QModelIndex& index, s32 role) const final;
    QVariant headerData(s32 section, Qt::Orientation orientation, s32 role) const final;

    void onTextureAdded(s32 index);
    void onTextureRemoved(s32 index);

private:
    void emitRowChangedFor(Ptcl::Texture* texture);

private:
    const Ptcl::TextureList* mTextures{nullptr};
};


// ========================================================================== //


} // namespace PtclEditor
