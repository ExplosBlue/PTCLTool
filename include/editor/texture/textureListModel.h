#pragma once

#include "ptcl/ptcl.h"

#include <QAbstractListModel>


namespace PtclEditor {


// ========================================================================== //


class TextureListModel final : public QAbstractListModel {
    Q_OBJECT
public:
    explicit TextureListModel(QObject* parent = nullptr);

    void setTextures(const Ptcl::TextureList* textures);

    s32 rowCount(const QModelIndex& parent = {}) const final;
    QVariant data(const QModelIndex& index, s32 role) const final;

    void onTextureAdded(s32 index);
    void onTextureRemoved(s32 index);

private:
    void emitRowChangedFor(Ptcl::Texture* texture);

private:
    const Ptcl::TextureList* mTextures{nullptr};
};


// ========================================================================== //


} // namespace PtclEditor
