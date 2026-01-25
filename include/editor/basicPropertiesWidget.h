#pragma once

#include "editor/enumComboBox.h"
#include "editor/sizedSpinBox.h"

#include "ptcl/ptclEmitter.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QWidget>


// ========================================================================== //


class BasicPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit BasicPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::Emitter::BasicProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::Emitter::BasicProperties& properties);

private:
    Ptcl::Emitter::BasicProperties mProps{};

    QLineEdit mNameLineEdit{};
    EnumComboBox<Ptcl::EmitterType> mTypeComboBox{};
    QComboBox mRandomSeedMode{};
    SizedSpinBox<u32> mRandomSeedSpinBox{};
    EnumComboBox<Ptcl::FollowType> mFollowTypeComboBox{};
    EnumComboBox<Ptcl::BillboardType> mBillboardComboBox{};
};


// ========================================================================== //
