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

    void setProperties(const Ptcl::TerminationProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::TerminationProperties& properties);

private:
    Ptcl::TerminationProperties mProps{};

    QCheckBox mIsStopEmitCheckBox;
    QDoubleSpinBox mAlphaAddInSpinBox;
};


// ========================================================================== //
