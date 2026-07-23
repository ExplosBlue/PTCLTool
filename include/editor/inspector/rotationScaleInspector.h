#pragma once

#include "editor/components/animGraph.h"
#include "editor/components/enumComboBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QComboBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class RotationScaleInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit RotationScaleInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;
    void setupConnections();
    void updateAxis();

    void updateAnimPoint(s32 pointIndex, const AnimGraph::GraphPoint& point, f32 (Math::Vector2f::*get)() const);
    void updateGraphs();

protected:
    void resizeEvent(QResizeEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    // Rotation widgets
    EnumComboBox<Ptcl::RotType> mRotTypeComboBox{};
    VectorSpinBox<Math::Vector3f> mInitRotSpinBox{};
    VectorSpinBox<Math::Vector3f> mInitRotRandSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotVelSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotVelRandSpinBox{};
    VectorSpinBox<Math::Vector2f> mRotBasisSpinBox{};

    // Scale widgets
    AnimGraph mGraphX{};
    AnimGraph mGraphY{};
    QDoubleSpinBox mScaleRandSpinbox{};
    QWidget* mOverlay = nullptr;
    const Ptcl::Emitter* mLastEmitter = nullptr;
};


// ========================================================================== //


} // namespace PtclEditor
