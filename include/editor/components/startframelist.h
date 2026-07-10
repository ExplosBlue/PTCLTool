#pragma once

#include "util/paintUtil.h"

#include <QFrame>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <array>


class ThumbnailWidget;

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
    void updateGrid();

private:
    static constexpr s32 sColumnCount = 4;
    static constexpr s32 sRowCount = 4;
    static constexpr s32 sMaxFrameCount = sColumnCount * sRowCount;
    static constexpr s32 sThumbSize = 48;

private:
    TextureFrameInfo mFrameInfo{};

    u16 mSlotCount{};
    u16 mSlotMax{};

    QSpinBox* mCountSpinBox{};
    QGridLayout* mGridLayout{};

    std::array<ThumbnailWidget*, sMaxFrameCount> mThumbnails{};
};


// ========================================================================== //


} // namespace PtclEditor
