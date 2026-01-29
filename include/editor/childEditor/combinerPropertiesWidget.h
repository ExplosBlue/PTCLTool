#pragma once

#include "editor/components/combinerPreviewWidget.h"
#include "editor/components/enumComboBox.h"
#include "editor/childEditor/childEditorWidget.h"

#include <QWidget>

namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget::CombinerPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit CombinerPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ChildData::CombinerProperties& properties);
    void setCombinerSrc(const Ptcl::TextureHandle* texture, const Ptcl::binColor3f* constant, const Ptcl::binColor4f* primary);

    void updateCombinerPreview();

signals:
    void propertiesUpdated(const Ptcl::ChildData::CombinerProperties& properties);

private:
    Ptcl::ChildData::CombinerProperties mProps{};

    EnumComboBox<Ptcl::BlendFuncType> mBlendFuncComboBox{};
    EnumComboBox<Ptcl::DepthFuncType> mDepthFuncComboBox{};
    EnumComboBox<Ptcl::ColorCombinerFuncType> mCombinerFuncComboBox{};
    CombinerPreviewWidget mCombinerPreview{};
};


// ========================================================================== //


} // namespace PtclEditor
