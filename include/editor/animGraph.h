#pragma once

#include "ptcl/ptclEmitter.h"

#include <QWidget>


// ========================================================================== //


class AnimGraph : public QWidget {
    Q_OBJECT
public:
    explicit AnimGraph(QWidget* parent = nullptr);

    void setAnim(const Ptcl::ScaleAnim& scaleAnim);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Ptcl::ScaleAnim mAnim;
};

// ========================================================================== //

