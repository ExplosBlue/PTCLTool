#pragma once

#include "editor/emitterWidget/emitterWidget.h"

#include "ptcl/ptclEmitter.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidget::TerminationPropertiesWidget final : public QWidget {
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


} // namespace PtclEditor
