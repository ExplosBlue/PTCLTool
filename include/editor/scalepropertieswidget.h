#pragma once

#include "ptcl/ptclEmitter.h"

#include "editor/animGraph.h"

#include <QWidget>


// ========================================================================== //

class scalePropertiesWidget : public QWidget {
    Q_OBJECT
public:
    explicit scalePropertiesWidget(QWidget* parent = nullptr);

    // TODO: Change this to setProperties, update ScaleAnim to include other scale properties
    void setAnim(const Ptcl::ScaleAnim& scaleAnim);

private:
    Ptcl::ScaleAnim mAnim;

    AnimGraph mGraphX;
    AnimGraph mGraphY;
};

// ========================================================================== //
