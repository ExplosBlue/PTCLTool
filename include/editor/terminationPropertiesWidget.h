#pragma once

#include "ptcl/ptclEmitter.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


// ========================================================================== //


class TerminationPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit TerminationPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::TerminationProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::Emitter::TerminationProperties& properties);

private:
    Ptcl::Emitter::TerminationProperties mProps{};

    QCheckBox mIsStopEmitCheckBox;
    QDoubleSpinBox mAlphaAddInSpinBox;
};


// ========================================================================== //
