#pragma once

#include "editor/enumComboBox.h"
#include "editor/vectorSpinBox.h"
#include "ptcl/ptclEmitter.h"

#include <QComboBox>
#include <QLabel>
#include <QWidget>


// ========================================================================== //


class RotationPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit RotationPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::RotationProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::RotationProperties& properties);

private:
    void populateWidgets();
    void updateAxis();

private:
    Ptcl::RotationProperties mProps{};

    EnumComboBox<Ptcl::RotType> mRotTypeSpinBox{};
    VectorSpinBox<QVector3D> mInitRotSpinBox{};
    VectorSpinBox<QVector3D> mInitRotRandSpinBox{};
    VectorSpinBox<QVector3D> mRotVelSpinBox{};
    VectorSpinBox<QVector3D> mRotVelRandSpinBox{};
};


// ========================================================================== //
