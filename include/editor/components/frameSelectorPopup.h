#pragma once

#include "typedefs.h"

#include <QPoint>
#include <QToolButton>
#include <QWidget>


struct TextureFrameInfo;


namespace PtclEditor {


// ========================================================================== //


class FrameSelectorPopup final : public QWidget {
    Q_OBJECT
public:
    struct SelectionState {
        s32 maxFrame = 0;
        s32 currentFrame = 0;
        s32 slotIndex = 0;
        QPoint anchorGlobal;
    };

public:
    explicit FrameSelectorPopup(const TextureFrameInfo& frameInfo, const SelectionState& state, QWidget* parent = nullptr);

signals:
    void frameSelected(s32 frameIdx);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void selectFrame(s32 frameIdx);
    void positionAt(const QPoint& point);
    void initialize(const TextureFrameInfo& frameInfo);

    QToolButton* makeFrameButton(const TextureFrameInfo& frameInfo, s32 frameIdx);

private:
    s32 mArrowCenter{0};

    const SelectionState mState{};

    static constexpr s32 sThumbSize = 28;
    static constexpr s32 sButtonPadding = 16;
    static constexpr s32 sLabelHeight = 18;
    static constexpr s32 sArrowHeight = 12;
    static constexpr s32 sNumColumn = 4;
    static constexpr s32 sPadding = 8;
};


// ========================================================================== //


} // namespace PtclEditor
