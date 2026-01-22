#pragma once

#include "ptcl/ptclEmitter.h"
#include "editor/enumComboBox.h"

#include <QWidget>


// ========================================================================== //


class CombinerPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit CombinerPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::CombinerProperties& properties);

signals:
    void propertiesUpdated(const Ptcl::CombinerProperties& properties);

private:
    Ptcl::CombinerProperties mProps{};

    EnumComboBox<Ptcl::BlendFuncType> mBlendFuncComboBox{};
    EnumComboBox<Ptcl::DepthFuncType> mDepthFuncComboBox{};
    EnumComboBox<Ptcl::ColorCombinerFuncType> mCombinerFuncComboBox{};
};


// ========================================================================== //
