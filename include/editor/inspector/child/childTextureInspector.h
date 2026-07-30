#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/sizedSpinBox.h"
#include "editor/components/texturePreview.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QLabel>
#include <QPushButton>
#include <QWidget>


namespace PtclEditor {


// ==========================================================================//


class ChildTextureInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ChildTextureInspector(QWidget* parent = nullptr);

private slots:
    void changeTexture();
    void onTextureChanged(s32 index) final;

private:
    void populateProperties() final;
    void setupConnections();
    void updateUVLabel();

private:
    TexturePreview mTexturePreview{};

    EnumComboBox<Ptcl::TextureWrap> mWrapTComboBox{};
    EnumComboBox<Ptcl::TextureWrap> mWrapSComboBox{};
    EnumComboBox<Ptcl::TextureFilter> mFilterComboBox{};
    SizedSpinBox<u8> mTexLodLevel{};
    SizedSpinBox<u8> mDivXSpinBox{};
    SizedSpinBox<u8> mDivYSpinBox{};
    SizedSpinBox<u16> mRepXSpinBox{};
    SizedSpinBox<u16> mRepYSpinBox{};
    QLabel* mUVLabel{};
    QPushButton mChangeTextureButton{};
    bool mUpdatingEmitter{false};
};


// ==========================================================================//


} // namespace PtclEditor
