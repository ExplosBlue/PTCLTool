#ifndef TEXTUREPROPERTIESWIDGET_H
#define TEXTUREPROPERTIESWIDGET_H

#include <QWidget>
#include <QComboBox>

#include "ptcl/ptclEmitter.h"

#include "editor/thumbnailWidget.h"
#include "editor/enumComboBox.h"

class TexturePropertiesWidget : public QWidget {
    Q_OBJECT
public:
    explicit TexturePropertiesWidget(QWidget* parent = nullptr);

    void setEmitter(Ptcl::Emitter* emitter);

    void populateWidgets();

private slots:
    void updateWrapT();
    void updateWrapS();
    void updateMinFilter();
    void updateMagFilter();
    void updateMipFilter();

private:
    Ptcl::Emitter* mEmitterPtr;
    ThumbnailWidget mTexturePreview;

    EnumComboBox<Ptcl::TextureWrap> mWrapTComboBox;
    EnumComboBox<Ptcl::TextureWrap> mWrapSComboBox;
    EnumComboBox<Ptcl::TextureFilter> mMagFilterComboBox;
    EnumComboBox<Ptcl::TextureFilter> mMinFilterComboBox;
    EnumComboBox<Ptcl::TextureMipFilter> mMipFilterComboBox;
};

#endif // TEXTUREPROPERTIESWIDGET_H
