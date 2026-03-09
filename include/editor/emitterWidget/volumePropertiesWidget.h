#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/emitterWidget/emitterWidgetBase.h"

#include <QComboBox>
#include <QLabel>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class VolumePropertiesWidget : public EmitterWidgetBase {
    Q_OBJECT
public:
    explicit VolumePropertiesWidget(QWidget* parent = nullptr);

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
