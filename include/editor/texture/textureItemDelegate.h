#pragma once

#include "typedefs.h"

#include <QStyledItemDelegate>


namespace PtclEditor {


// ========================================================================== //


class TextureItemDelegate final : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit TextureItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const final;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const final;

private:
    static constexpr s32 sThumbSize = 64;
    static constexpr s32 sPaddingH = 8;
    static constexpr s32 sPaddingV = 4;
};


// ========================================================================== //


} // namespace PtclEditor
