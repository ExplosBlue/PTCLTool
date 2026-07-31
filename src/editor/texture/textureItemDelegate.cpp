#include "editor/texture/textureItemDelegate.h"

#include "util/paintUtil.h"

#include <QPainter>

#include <algorithm>


namespace PtclEditor {


// ========================================================================== //


TextureItemDelegate::TextureItemDelegate(QObject* parent) :
    QStyledItemDelegate{parent} {}

void TextureItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();

    if (option.state & QStyle::State_Selected) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    if (option.state & QStyle::State_MouseOver) {
        painter->fillRect(option.rect, option.palette.highlight());
    }

    const QRect thumbRect(
        option.rect.left() + sPaddingH,
        option.rect.top() + (option.rect.height() - sThumbSize) / 2,
        sThumbSize,
        sThumbSize
    );

    const auto pixmap = index.data(Qt::DecorationRole).value<QPixmap>();
    PaintUtil::drawTextureThumbnail(*painter, thumbRect, pixmap);

    const QRect textRect(
        thumbRect.right() + sPaddingH,
        option.rect.top(),
        option.rect.width() - thumbRect.width() - (2 * sPaddingH),
        option.rect.height()
    );

    const auto text = index.data(Qt::DisplayRole).toString();
    painter->setPen(option.palette.text().color());
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextWordWrap, text);

    painter->restore();
}

QSize TextureItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    const QString text = index.data(Qt::DisplayRole).toString();
    QFontMetrics fm(option.font);

    QRect textRect = fm.boundingRect(QRect(0, 0, 1000, 1000), Qt::TextWordWrap, text);
    s32 width = sPaddingH + sThumbSize + sPaddingH + textRect.width() + sPaddingH;
    s32 height = std::max(sThumbSize, textRect.height()) + (2 * sPaddingV);

    return {width, height};
}


// ========================================================================== //


} // namespace PtclEditor
