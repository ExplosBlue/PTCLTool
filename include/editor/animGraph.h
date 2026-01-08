#pragma once

#include "ptcl/ptclEmitter.h"

#include <QWidget>


// ========================================================================== //

struct GraphRange {
    f32 min;
    f32 max;
    f32 tickStep;
};

// ========================================================================== //

class AnimGraph : public QWidget {
    Q_OBJECT
public:
    explicit AnimGraph(QWidget* parent = nullptr);

    void setAnim(const Ptcl::ScaleAnim& scaleAnim);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    static void drawAxisLabels(QPainter& painter, s32 width, s32 height, s32 xDivs, const GraphRange& range);
    static void drawGrid(QPainter& painter, s32 width, s32 height, const GraphRange& range);

    static GraphRange computeGraphRange(const std::initializer_list<f32>& values, s32 maxTicks);
    static f32 chooseTickStep(f32 range, s32 maxTicks);

private:
    Ptcl::ScaleAnim mAnim;

    // Ui Style
    static constexpr QColor sColorGridBg = { 48, 48, 48 };
    static constexpr QColor sColorGridlineMajor = { 26, 26, 26 };
    static constexpr QColor sColorGridlineMinor = { 42, 42, 42 };

    static constexpr QColor sColorHandle = { 0, 0, 0 };
    static constexpr QColor sColorHandleActive = { 253, 133, 2 };

    static constexpr QColor sColorAxisX = { 238, 51, 79 };
    static constexpr QColor sColorAxisY = { 42, 125, 212 };
    static constexpr QColor sColorAxisZ = { 137, 214, 1 };

};

// ========================================================================== //

