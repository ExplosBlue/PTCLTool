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

    void setProperties(const Ptcl::GravityProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::GravityProperties& properties);

private:
    Ptcl::GravityProperties mProps{};

    QCheckBox mIsDirectionalCheckBox{};
    VectorSpinBox<Math::Vector3f> mGravitySpinBox{};

};


// ========================================================================== //
