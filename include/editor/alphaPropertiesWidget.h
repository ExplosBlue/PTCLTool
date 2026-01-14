#pragma once

#include "ptcl/ptclEmitter.h"

#include "editor/animGraph.h"

#include <QWidget>


// ========================================================================== //


class AlphaPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit AlphaPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::AlphaProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::AlphaProperties& properties);

private:
    Ptcl::AlphaProperties mProps{};

    AnimGraph mGraphA{};
};


// ========================================================================== //
