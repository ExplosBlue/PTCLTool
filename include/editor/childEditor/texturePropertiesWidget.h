#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/thumbnailWidget.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/childEditor/childEditorWidget.h"

#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget::TexturePropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit TexturePropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ChildData::TextureProperties& properties, const std::shared_ptr<Ptcl::Texture>& texture);
    void setTextureList(const Ptcl::TextureList* textureList);

signals:
    void propertiesUpdated(const Ptcl::ChildData::TextureProperties& properties);
    void textureUpdated(const std::shared_ptr<Ptcl::Texture>& oldTexture, const std::shared_ptr<Ptcl::Texture>& newTexture);

private slots:
    void changeTexture();

private:
    void setupConnections();

private:
    Ptcl::ChildData::TextureProperties mProps{};
    std::shared_ptr<Ptcl::Texture> mTexture{};

    const Ptcl::TextureList* mTextureList{nullptr};

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
