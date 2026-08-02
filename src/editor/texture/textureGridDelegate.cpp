#include "editor/texture/textureGridDelegate.h"

#include "util/paintUtil.h"

#include <QPainter>


namespace PtclEditor {


// ========================================================================== //


TextureGridDelegate::TextureGridDelegate(s32 cellSize, QObject* parent) :
    QStyledItemDelegate{parent}, mCellSize{cellSize} {}

void TextureGridDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    } else if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    const QRect thumbRect = option.rect.adjusted(sPadding, sPadding, -sPadding, -sPadding);

    const auto pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
    PaintUtil::drawTextureThumbnail(*painter, thumbRect, pixmap);

    painter->restore();
}

QSize TextureGridDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    Q_UNUSED(option);
    Q_UNUSED(index);
    return {mCellSize, mCellSize};
}


// ========================================================================== //


} // namespace PtclEditor
