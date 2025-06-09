#pragma once

#include "editor/enumComboBox.h"
#include "editor/sizedSpinBox.h"
#include "editor/thumbnailWidget.h"
#include "editor/vectorSpinBox.h"
#include "ptcl/ptcl.h"
#include "ptcl/ptclEmitter.h"

#include <QTableWidget>
#include <QVector2D>
#include <QWidget>


// ========================================================================== //


class TexturePropertiesWidget : public QWidget {
    Q_OBJECT
public:
    explicit TexturePropertiesWidget(QWidget* parent = nullptr);

    void setEmitter(Ptcl::Emitter* emitter);
    void populateWidgets();
    void setTextureList(const Ptcl::TextureList* textureList);

signals:
    void textureUpdated(int oldIndex, int index);

private slots:
    void updateWrapT();
    void updateWrapS();
    void updateMinFilter();
    void updateMagFilter();
    void updateMipFilter();

    void updateNumTexPat();
    void updateTexDivX();
    void updateTexDivY();
    void updateTexPatFreq();
    void updateTexPatTblUse();

    void updateTexPatTbl(QTableWidgetItem* item);
    void updateUVScale();
    void changeTexture();

private:
    void updateTextureDetails();
    void updateTexPatTblColumns();

private:
    Ptcl::Emitter* mEmitterPtr;
    const Ptcl::TextureList* mTextureList;

    ThumbnailWidget mTexturePreview;

    EnumComboBox<Ptcl::TextureWrap> mWrapTComboBox;
    EnumComboBox<Ptcl::TextureWrap> mWrapSComboBox;
    EnumComboBox<Ptcl::TextureFilter> mMagFilterComboBox;
    EnumComboBox<Ptcl::TextureFilter> mMinFilterComboBox;
    EnumComboBox<Ptcl::TextureMipFilter> mMipFilterComboBox;

    SizedSpinBox<u16> mNumTexPat;
    SizedSpinBox<u8> mTexDivX;
    SizedSpinBox<u8> mTexDivY;
    SizedSpinBox<u16> mTexPatFreq;
    SizedSpinBox<u16> mTexPatTblUse;

    QTableWidget mTexPatTbl;

    VectorSpinBox<QVector2D> mUVScaleSpinBox;

    bool mIsPopulating;
};


// ========================================================================== //
