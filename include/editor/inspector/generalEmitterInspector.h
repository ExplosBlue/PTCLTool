#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/sizedSpinBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class GeneralEmitterInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    enum class ShapeType {
        Particle,
        Stripe,
        Primitive
    };

public:
    explicit GeneralEmitterInspector(QWidget* parent = nullptr);

private:
    void populateProperties() final;

    void setupConnections();
    void updateShapeRowVisibility();
    void updateBillboardFilter();
    void updateStripeSectionVisibility();

    void setBillboardType(Ptcl::BillboardType type, const QString& label, const QString& key);

    static bool isBillboardAllowedForShape(Ptcl::BillboardType billboard, ShapeType shape);
    static ShapeType shapeFromBillboard(Ptcl::BillboardType type);

private:
    QFormLayout* mMainLayout{nullptr};

    QLineEdit mNameLineEdit{};
    EnumComboBox<Ptcl::EmitterType> mTypeComboBox{};
    EnumComboBox<PtclSeed::Mode> mRandomSeedMode{};
    SizedSpinBox<u32> mRandomSeedSpinBox{};
    EnumComboBox<Ptcl::FollowType> mFollowTypeComboBox{};
    EnumComboBox<ShapeType> mShapeComboBox{};
    EnumComboBox<Ptcl::BillboardType> mBillboardTypeComboBox{};

    QWidget* mStripeSection{nullptr};
    EnumComboBox<Ptcl::StripeType> mStripeTypeComboBox{};
    QCheckBox mStripeEmitterCoordCheckBox{};
    QSpinBox mStripeNumHistSpinBox{};
    QDoubleSpinBox mStripeStartAlphaSpinBox{};
    QDoubleSpinBox mStripeEndAlphaSpinBox{};
    VectorSpinBox<Math::Vector2f> mStripeUVScrollSpeedSpinBox{};
    QSpinBox mStripeHistStepSpinBox{};
    QDoubleSpinBox mStripeDirInterpolateSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
