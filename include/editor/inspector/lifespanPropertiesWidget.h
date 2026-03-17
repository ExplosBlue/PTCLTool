#pragma once

#include "editor/inspector/inspectorWidgetBase.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class LifespanPropertiesWidget final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit LifespanPropertiesWidget(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QCheckBox mInfiniteLifeCheckBox;
    QSpinBox mLifeSpanSpinBox;
    QSpinBox mLifeSpanRndSpinBox;

    static constexpr s32 sLifeInfinite = std::numeric_limits<s32>::max();
};


// ========================================================================== //


} // namespace PtclEditor
