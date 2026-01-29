#pragma once

#include "ptcl/ptcl.h"
#include "ptcl/ptclChildData.h"

#include <QCheckBox>
#include <QSpinBox>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ChildEditorWidget(QWidget* parent = nullptr);

    void setChildData(Ptcl::ChildData* childData, const BitFlag<Ptcl::ChildFlag>& childFlag);
    void setTextureList(const Ptcl::TextureList* textureList);
    void setParentColor0(const Ptcl::binColor4f& parentColor0);

    void clear();

signals:
    void flagsUpdated(const BitFlag<Ptcl::ChildFlag>& childFlag);
    void textureUpdated(const std::shared_ptr<Ptcl::Texture>& oldTexture, const std::shared_ptr<Ptcl::Texture>& newTexture);

private:
    void setupLayout(QVBoxLayout* mainLayout);
    void setupConnections();
    void setCombinerPropertiesSrc();

private:
    class BasicPropertiesWidget;
    class EmissionPropertiesWidget;
    class VelocityPropertiesWidget;
    class RotationPropertiesWidget;
    class ScalePropertiesWidget;
    class TexturePropertiesWidget;
    class ColorPropertiesWidget;
    class AlphaPropertiesWidget;
    class CombinerPropertiesWidget;

private:
    Ptcl::ChildData* mDataPtr{nullptr};
    BitFlag<Ptcl::ChildFlag> mChildFlag{};

    const Ptcl::TextureList* mTextureList{nullptr};
    Ptcl::binColor4f mParentColor0{};

    QCheckBox mEnabledCheckbox{};
    QWidget* mSectionsContainer{};
    BasicPropertiesWidget* mBasicProperties{nullptr};
    EmissionPropertiesWidget* mEmissionProperties{nullptr};
    VelocityPropertiesWidget* mVelocityProperties{nullptr};
    RotationPropertiesWidget* mRotationProperties{nullptr};
    ScalePropertiesWidget* mScaleProperties{nullptr};
    TexturePropertiesWidget* mTextureProperties{nullptr};
    ColorPropertiesWidget* mColorProperties{nullptr};
    AlphaPropertiesWidget* mAlphaProperties{nullptr};
    CombinerPropertiesWidget* mCombinerProperties{nullptr};
};


// ========================================================================== //


} // namespace PtclEditor
