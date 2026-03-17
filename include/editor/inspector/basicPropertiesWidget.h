#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/sizedSpinBox.h"
#include "editor/inspector/inspectorWidgetBase.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class BasicPropertiesWidget final : public InspectorWidgetBase {
    Q_OBJECT
private:
    static constexpr std::array<Ptcl::BillboardType, 6> sBillboardTypes{
        Ptcl::BillboardType::Billboard,
        Ptcl::BillboardType::PolygonXY,
        Ptcl::BillboardType::PolygonXZ,
        Ptcl::BillboardType::VelLook,
        Ptcl::BillboardType::VelLookPolygon,
        Ptcl::BillboardType::BillboardY,
    };

    static constexpr std::array<Ptcl::BillboardType, 2> sStripeTypes{
        Ptcl::BillboardType::Stripe,
        Ptcl::BillboardType::ComplexStripe
    };

    enum class ShapeType {
        Particle,
        Stripe,
        Primitive
    };

public:
    explicit BasicPropertiesWidget(QWidget* parent = nullptr);

private:
    void populateProperties() final;

    void setupConnections();
    void updateShapeRowVisibility();
    void updateBillboardRowVisibility();
    void syncBillboardSelection();

    void setBillboardType(Ptcl::BillboardType type, const QString& label, const QString& key);

    static bool isBillboardAllowedForShape(Ptcl::BillboardType billboard, ShapeType shape);
    static ShapeType shapeFromBillboard(Ptcl::BillboardType type);

private:
    QFormLayout* mMainLayout{nullptr};

    QLineEdit mNameLineEdit{};
    EnumComboBox<Ptcl::EmitterType> mTypeComboBox{};
    QComboBox mRandomSeedMode{};
    SizedSpinBox<u32> mRandomSeedSpinBox{};
    EnumComboBox<Ptcl::FollowType> mFollowTypeComboBox{};
    QComboBox mBillboardComboBox{};
    QComboBox mShapeComboBox{};
    QComboBox mStripeTypeComboBox{};
    QCheckBox mIsBillboardMtxCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
