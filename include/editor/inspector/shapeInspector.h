#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ShapeInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit ShapeInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();
    void setupUi();
    void updateVolumeFieldVisibility(Ptcl::VolumeType volumeType);

private:
    struct VolumeField {
        QWidget* widget{};
        std::function<QString(Ptcl::VolumeType)> label{};
        std::function<bool(Ptcl::VolumeType)> isVisible{};
    };

private:
    // Volume
    std::vector<VolumeField> mVolumeFields{};
    std::unordered_map<QWidget*, QLabel*> mVolumeFieldLabels{};

    QComboBox mVolumeTblIndexComboBox{};
    EnumComboBox<Ptcl::VolumeType> mVolumeTypeComboBox{};
    VectorSpinBox<Math::Vector3f> mRadiusSpinBox{};
    QDoubleSpinBox mArcWidthSpinBox{};
    QDoubleSpinBox mArcStartSpinBox{};
    QDoubleSpinBox mLengthSpinBox{};

    // Transform
    VectorSpinBox<Math::Vector3f> mTranslationSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotationSpinBox{};
    VectorSpinBox<Math::Vector3f> mScaleSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
