#pragma once

#include "editor/vectorSpinBox.h"
#include "ptcl/ptclEmitter.h"

#include <QCheckBox>
#include <QWidget>


// ========================================================================== //


class GravityPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit GravityPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::GravityProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::Emitter::GravityProperties& properties);

private:
    Ptcl::Emitter::GravityProperties mProps{};

    QCheckBox mIsDirectionalCheckBox{};
    VectorSpinBox<Math::Vector3f> mGravitySpinBox{};

};


// ========================================================================== //
