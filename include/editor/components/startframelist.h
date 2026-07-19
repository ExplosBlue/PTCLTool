#pragma once

#include "util/paintUtil.h"

#include <QFrame>
#include <QLabel>
#include <QListWidget>
#include <QSpinBox>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


class StartFrameList final : public QFrame {
    Q_OBJECT
public:
    explicit StartFrameList(QWidget* parent = nullptr);

    void setSource(const TextureFrameInfo& frameInfo);

    void setSlotCount(u16 count);
    void setSlotMax(u16 count);

    u16 slotCount() const;

signals:
    void slotCountChanged(u16 count);

private:
    QPixmap renderThumb(s32 frameIndex) const;
    void updateList();
    s32 expandedHeight() const;
    void updateZeroFrameLabel();

private:
    static constexpr s32 sMaxFrameCount = 16;
    static constexpr s32 sColumns = 4;
    static constexpr s32 sThumbSize = 48;
    static constexpr s32 sItemSize = sThumbSize + 16;
    static constexpr s32 sSpacing = 8;

private:
    TextureFrameInfo mFrameInfo{};

    u16 mSlotCount{};
    u16 mSlotMax{};

    QSpinBox* mCountSpinBox{};
    QToolButton* mExpandButton{};
    QListWidget* mListWidget{};
    QLabel* mZeroFrameLabel{};
};


// ==========================================================================//


} // namespace PtclEditor
