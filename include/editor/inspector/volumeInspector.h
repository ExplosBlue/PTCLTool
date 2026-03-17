#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QComboBox>
#include <QLabel>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class VolumeInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit VolumeInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();
    void setupUi();
    void updateFieldVisibility(Ptcl::VolumeType volumeType);

private:
    struct VolumeField {
        QWidget* widget{};
        std::function<QString(Ptcl::VolumeType)> label{};
        std::function<bool(Ptcl::VolumeType)> isVisible{};
    };

private:
    std::vector<VolumeField> mFields{};
    std::unordered_map<QWidget*, QLabel*> mFieldLabels{};

    QComboBox mVolumeTblIndexComboBox{};
    EnumComboBox<Ptcl::VolumeType> mTypeComboBox{};
    VectorSpinBox<Math::Vector3f> mRadiusSpinBox{};
    QDoubleSpinBox mSweepStartSpinBox{};
    QDoubleSpinBox mSweepParamSpinBox{};
    QDoubleSpinBox mLengthSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
