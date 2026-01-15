#pragma once

#include "editor/enumComboBox.h"
#include "editor/vectorSpinBox.h"
#include "ptcl/ptclEmitter.h"

#include <QComboBox>
#include <QLabel>
#include <QWidget>


// ========================================================================== //


class VolumePropertiesWidget : public QWidget {
    Q_OBJECT
public:
    explicit VolumePropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::VolumeProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::VolumeProperties& properties);

private:
    void setupUi();
    void setupSignals();
    void populateWidgets();
    void updateFieldVisibility(Ptcl::VolumeType volumeType);

private:
    struct VolumeField {
        QWidget* widget;
        std::function<QString(Ptcl::VolumeType)> label;
        std::function<bool(Ptcl::VolumeType)> isVisible;
    };

private:
    Ptcl::VolumeProperties mProps{};

    std::vector<VolumeField> mFields;
    std::unordered_map<QWidget*, QLabel*> mFieldLabels;

    QComboBox mVolumeTblIndexComboBox;
    EnumComboBox<Ptcl::VolumeType> mTypeComboBox;
    VectorSpinBox<Math::Vector3f> mRadiusSpinBox;
    QDoubleSpinBox mSweepStartSpinBox;
    QDoubleSpinBox mSweepParamSpinBox;
    QDoubleSpinBox mLengthSpinBox;
};


// ========================================================================== //
