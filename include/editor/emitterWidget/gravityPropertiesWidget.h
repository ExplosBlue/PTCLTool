#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/emitterWidget/emitterWidget.h"

#include "ptcl/ptclEmitter.h"

#include <QCheckBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidget::GravityPropertiesWidget final : public QWidget {
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


} // namespace PtclEditor
