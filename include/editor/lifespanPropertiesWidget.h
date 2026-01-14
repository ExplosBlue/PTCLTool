#pragma once

#include "ptcl/ptclEmitter.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


// ========================================================================== //


class LifespanPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit LifespanPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::LifespanProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::LifespanProperties& properties);

private:
    Ptcl::LifespanProperties mProps{};

    QCheckBox mInfiniteLifeCheckBox;
    QSpinBox mLifeSpanSpinBox;
    QSpinBox mLifeSpanRndSpinBox;

    static constexpr s32 sLifeInfinite = std::numeric_limits<s32>::max();
};


// ========================================================================== //
