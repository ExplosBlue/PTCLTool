#pragma once

#include "editor/enumComboBox.h"
#include "editor/vectorSpinBox.h"
#include "ptcl/ptclEmitter.h"

#include <QComboBox>
#include <QLabel>
#include <QWidget>


// ========================================================================== //


class RotationPropertiesWidget : public QWidget {
    Q_OBJECT
public:
    explicit RotationPropertiesWidget(QWidget* parent = nullptr);

    void setEmitter(Ptcl::Emitter* emitter);

private:
    void populateWidgets();
    void updateAxis();

private:
    Ptcl::Emitter* mEmitterPtr;

    EnumComboBox<Ptcl::RotType> mRotTypeSpinBox;

    VectorSpinBox<QVector3D> mInitRotSpinBox;
    VectorSpinBox<QVector3D> mInitRotRandSpinBox;
    VectorSpinBox<QVector3D> mRotVelSpinBox;
    VectorSpinBox<QVector3D> mRotVelRandSpinBox;

    bool misPopulating = false;
};


// ========================================================================== //
