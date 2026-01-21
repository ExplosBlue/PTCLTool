#pragma once

#include "editor/enumComboBox.h"
#include "editor/sizedSpinBox.h"
#include "editor/thumbnailWidget.h"
#include "editor/vectorSpinBox.h"
#include "ptcl/ptcl.h"
#include "ptcl/ptclEmitter.h"

#include <QCheckBox>
#include <QTableWidget>
#include <QVector2D>
#include <QWidget>


// ========================================================================== //


class TexturePropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit TexturePropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::TextureProperties& properties, const std::shared_ptr<Ptcl::Texture>& texture);

    void populateWidgets();
    void setTextureList(const Ptcl::TextureList* textureList);

signals:
    void propertiesUpdated(const Ptcl::TextureProperties& properties);
    void textureUpdated(const std::shared_ptr<Ptcl::Texture>& oldTexture, const std::shared_ptr<Ptcl::Texture>& newTexture);

private slots:
    void changeTexture();

private:
    void updateTextureDetails();
    void updateTexPatTblColumns();

private:
    Ptcl::TextureProperties mProps{};
    std::shared_ptr<Ptcl::Texture> mTexture{};

    const Ptcl::TextureList* mTextureList{nullptr};

    ThumbnailWidget mTexturePreview{};

    EnumComboBox<Ptcl::TextureWrap> mWrapTComboBox{};
    EnumComboBox<Ptcl::TextureWrap> mWrapSComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mMagFilterComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mMinFilterComboBox{};
    EnumComboBox<Ptcl::TextureMipFilter> mMipFilterComboBox{};

    SizedSpinBox<u16> mNumTexPat{};
    SizedSpinBox<u8> mTexDivX{};
    SizedSpinBox<u8> mTexDivY{};
    SizedSpinBox<u16> mTexPatFreq{};
    SizedSpinBox<u16> mTexPatTblUse{};

    QTableWidget mTexPatTbl{1, 16, this};

    VectorSpinBox<Math::Vector2f> mUVScaleSpinBox{};

    QCheckBox mTexPatAnimCheckBox{};
};


// ========================================================================== //
