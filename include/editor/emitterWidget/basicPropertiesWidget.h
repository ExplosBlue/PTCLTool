#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/sizedSpinBox.h"
#include "editor/emitterWidget/emitterWidget.h"

#include "ptcl/ptclEmitter.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidget::BasicPropertiesWidget final : public QWidget {
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

    void setDocument(Ptcl::Document* document);
    void setSelection(Ptcl::Selection* selection);

signals:
    void propertiesUpdated(const Ptcl::Emitter::BasicProperties& properties);
    void emitterTypeChanged();
    void emitterNameChanged();

private:
    void populateProperties();
    void setupConnections();
    void updateShapeRowVisibility();
    void updateBillboardRowVisibility();
    void syncBillboardSelection();

    void setBillboardType(Ptcl::BillboardType type, const QString& label, const QString& key);

    QString formatLabel(const QString& label) const;

    template<typename T, typename Getter, typename Setter>
    void setEmitterProperty(const QString& label, QString key, Getter getter, Setter setter, const T& value) {
        if (!mDocument || !mSelection) {
            return;
        }

        mDocument->setEmitterProperty<T>(mSelection->emitterSetIndex(), mSelection->emitterIndex(), formatLabel(label), key, getter, setter, value);
    }

    static bool isBillboardAllowedForShape(Ptcl::BillboardType billboard, ShapeType shape);
    static ShapeType shapeFromBillboard(Ptcl::BillboardType type);

private slots:
    void onEmitterChanged(s32 setIndex, s32 emitterIndex);

private:
    Ptcl::Document* mDocument{nullptr};
    const Ptcl::Selection* mSelection{nullptr};
    Ptcl::Emitter* mEmitter{nullptr};

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
