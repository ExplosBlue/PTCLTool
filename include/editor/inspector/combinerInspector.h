#pragma once

#include "editor/components/combinerPreviewWidget.h"
#include "editor/components/enumComboBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QWidget>
#include <QCheckBox>


namespace PtclEditor {


// ========================================================================== //


class CombinerInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit CombinerInspector(QWidget* parent = nullptr);

    void setDocument(Ptcl::Document* document) override;

private:
    void populateProperties() final;
    void setupConnections();

private:
    QCheckBox mFogCheckBox{};
    EnumComboBox<Ptcl::BlendFuncType> mBlendFuncComboBox{};
    EnumComboBox<Ptcl::DepthFuncType> mDepthFuncComboBox{};
    EnumComboBox<Ptcl::ColorCombinerFuncType> mCombinerFuncComboBox{};
    CombinerPreviewWidget mCombinerPreview{};

    Gfx::Color mPrimarySrc{};
};


// ========================================================================== //


} // namespace PtclEditor
