#pragma once

#include "ptcl/ptclEmitter.h"

#include <QWidget>


// ========================================================================== //

struct GraphRange {
    f32 min;
    f32 max;
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
    static void drawGrid(QPainter& painter, s32 width, s32 height);
    static GraphRange computeGraphRange(const std::initializer_list<f32>& values);

private:
    Ptcl::ScaleAnim mAnim;
};

// ========================================================================== //

