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

    void setEmitter(Ptcl::Emitter* emitter);

private:
    void populateWidgets();
    void updateFieldVisibility(Ptcl::VolumeType volumeType);

private:
    struct VolumeField {
        QWidget* widget;
        std::function<QString(Ptcl::VolumeType)> label;
        std::function<bool(Ptcl::VolumeType)> isVisible;
    };

private:
    Ptcl::Emitter* mEmitterPtr;

    std::vector<VolumeField> mFields;
    std::unordered_map<QWidget*, QLabel*> mFieldLabels;

    QComboBox mVolumeTblIndexComboBox;
    EnumComboBox<Ptcl::VolumeType> mTypeComboBox;
    VectorSpinBox<QVector3D> mRadiusSpinBox;
    QDoubleSpinBox mSweepStartSpinBox;
    QDoubleSpinBox mSweepParamSpinBox;
    QDoubleSpinBox mLengthSpinBox;

    bool misPopulating = false;
};


// ========================================================================== //
