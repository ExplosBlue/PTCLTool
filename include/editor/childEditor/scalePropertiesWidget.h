#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/childEditor/childEditorWidget.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget::ScalePropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ScalePropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ChildData::ScaleProperties& properties, bool inheritScale);

signals:
    void propertiesUpdated(const Ptcl::ChildData::ScaleProperties& properties);
    void inheritScaleUpdated(bool inherit);

private:
    void setupConnections();

private:
    Ptcl::ChildData::ScaleProperties mProps{};

    VectorSpinBox<Math::Vector2f> mScaleSpinBox{};
    VectorSpinBox<Math::Vector2f> mScaleTargetSpinBox{};
    QDoubleSpinBox mInheritRateSpinBox{};
    QSpinBox mStartFrameSpinBox{};
    QCheckBox mInheritScaleCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
