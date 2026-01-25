#pragma once

#include "editor/emitterWidget/emitterWidget.h"

#include "ptcl/ptclEmitter.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidget::LifespanPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit LifespanPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::LifespanProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::Emitter::LifespanProperties& properties);

private:
    Ptcl::Emitter::LifespanProperties mProps{};

    QCheckBox mInfiniteLifeCheckBox;
    QSpinBox mLifeSpanSpinBox;
    QSpinBox mLifeSpanRndSpinBox;

    static constexpr s32 sLifeInfinite = std::numeric_limits<s32>::max();
};


// ========================================================================== //


} // namespace PtclEditor
