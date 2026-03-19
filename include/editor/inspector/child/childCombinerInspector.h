#pragma once

#include "editor/components/combinerPreviewWidget.h"
#include "editor/components/enumComboBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildCombinerInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ChildCombinerInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    EnumComboBox<Ptcl::BlendFuncType> mBlendFuncComboBox{};
    EnumComboBox<Ptcl::DepthFuncType> mDepthFuncComboBox{};
    EnumComboBox<Ptcl::ColorCombinerFuncType> mCombinerFuncComboBox{};
    CombinerPreviewWidget mCombinerPreview{};
};


// ========================================================================== //


} // namespace PtclEditor
