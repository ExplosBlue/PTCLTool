#pragma once

#include "typedefs.h"

#include <QStyledItemDelegate>


namespace PtclEditor {


// ========================================================================== //


class TextureGridDelegate final : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit TextureGridDelegate(s32 cellSize = sCellSize, QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const final;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const final;

private:
    static constexpr s32 sCellSize = 96;
    static constexpr s32 sPadding = 8;

    s32 mCellSize{sCellSize};
};


// ========================================================================== //


} // namespace PtclEditor
