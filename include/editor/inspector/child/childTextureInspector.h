#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/thumbnailWidget.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildTextureInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ChildTextureInspector(QWidget* parent = nullptr);

private slots:
    void changeTexture();

private:
    void populateProperties() final;
    void setupConnections();

private:
    ThumbnailWidget mTexturePreview{};

    EnumComboBox<Ptcl::TextureWrap> mWrapTComboBox{};
    EnumComboBox<Ptcl::TextureWrap> mWrapSComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mMagFilterComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mMinFilterComboBox{};
    EnumComboBox<Ptcl::TextureMipFilter> mMipFilterComboBox{};
    VectorSpinBox<Math::Vector2f> mUVScaleSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
