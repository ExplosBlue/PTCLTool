#pragma once

#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class GravityInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit GravityInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();

private:
    QCheckBox mIsDirectionalCheckBox{};
    VectorSpinBox<Math::Vector3f> mGravitySpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
