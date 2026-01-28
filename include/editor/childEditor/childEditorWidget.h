#pragma once

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
    // void setTextureList(const Ptcl::TextureList* textureList);

    void clear();

signals:
    void flagsUpdated(const BitFlag<Ptcl::ChildFlag>& childFlag);

private:
    void setupLayout(QVBoxLayout* mainLayout);
    void setupConnections();

private:
    class BasicPropertiesWidget;
    class EmissionPropertiesWidget;
    class VelocityPropertiesWidget;
    class RotationPropertiesWidget;
    class ScalePropertiesWidget;

private:
    Ptcl::ChildData* mDataPtr{nullptr};
    BitFlag<Ptcl::ChildFlag> mChildFlag{};

    // const Ptcl::TextureList* mTextureList{nullptr};
    QCheckBox mEnabledCheckbox{};
    QWidget* mSectionsContainer{};
    BasicPropertiesWidget* mBasicProperties{nullptr};
    EmissionPropertiesWidget* mEmissionProperties{nullptr};
    VelocityPropertiesWidget* mVelocityProperties{nullptr};
    RotationPropertiesWidget* mRotationProperties{nullptr};
    ScalePropertiesWidget* mScaleProperties{nullptr};
};


// ========================================================================== //


} // namespace PtclEditor
