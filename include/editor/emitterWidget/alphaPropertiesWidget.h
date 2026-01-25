#pragma once

#include "editor/components/animGraph.h"
#include "editor/emitterWidget/emitterWidget.h"

#include "ptcl/ptclEmitter.h"

#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidget::AlphaPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit AlphaPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::AlphaProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::Emitter::AlphaProperties& properties);

private:
    Ptcl::Emitter::AlphaProperties mProps{};

    AnimGraph mGraphA{};
};


// ========================================================================== //


} // namespace PtclEditor
