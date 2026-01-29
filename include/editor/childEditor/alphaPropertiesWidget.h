#pragma once

#include "editor/childEditor/childEditorWidget.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget::AlphaPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit AlphaPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ChildData::AlphaProperties& properties, bool inheritAlpha);

signals:
    void propertiesUpdated(const Ptcl::ChildData::AlphaProperties& properties);
    void inheritAlphaUpdated(bool inherit);

private:
    void setupConnections();

private:
    Ptcl::ChildData::AlphaProperties mProps{};

    QDoubleSpinBox mAlphaSpinBox{};
    QDoubleSpinBox mAlphaTargetSpinBox{};
    QDoubleSpinBox mAlphaInitSpinBox{};
    QSpinBox mStartFrameSpinBox{};
    QSpinBox mBaseFrameSpinBox{};
    QCheckBox mInheritAlphaCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
