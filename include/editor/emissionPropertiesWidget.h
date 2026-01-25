#pragma once

#include "ptcl/ptclEmitter.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


// ========================================================================== //


class EmissionPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit EmissionPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::EmissionProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::Emitter::EmissionProperties& properties);

private:
    Ptcl::Emitter::EmissionProperties mProps{};

    QSpinBox mStartFrameSpinBox;
    QSpinBox mEndFrameSpinBox;
    QCheckBox mInfiniteEmitCheckBox;
    QSpinBox mLifeStepSpinBox;
    QSpinBox mLifeStepRndSpinBox;
    QSpinBox mEmitRateSpinBox;

    static constexpr s32 sEmitInfinite = std::numeric_limits<s32>::max();
};


// ========================================================================== //
