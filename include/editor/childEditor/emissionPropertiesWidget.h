#pragma once

#include "editor/childEditor/childEditorWidget.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget::EmissionPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit EmissionPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ChildData::EmissionProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::ChildData::EmissionProperties& properties);

private:
    Ptcl::ChildData::EmissionProperties mProps{};

    QSpinBox mEmitRateSpinBox{};
    QSpinBox mEmitTimingSpinBox{};
    QSpinBox mLifeSpinBox{};
    QSpinBox mEmitStepSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
