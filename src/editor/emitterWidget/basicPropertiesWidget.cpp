#include "editor/emitterWidget/basicPropertiesWidget.h"
#include "util/nameValidator.h"

#include <QLabel>

namespace PtclEditor {


// ========================================================================== //


EmitterWidget::BasicPropertiesWidget::BasicPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    for (Ptcl::BillboardType type : sBillboardTypes) {
        mBillboardComboBox.addItem(Ptcl::toString(type), QVariant::fromValue(type));
    }

    for (Ptcl::BillboardType type : sStripeTypes) {
        mStripeTypeComboBox.addItem(Ptcl::toString(type), QVariant::fromValue(type));
    }

    mNameLineEdit.setValidator(new EmitterNameValidator(&mNameLineEdit));

    mRandomSeedMode.addItem("Random Per Emitter", static_cast<s32>(PtclSeed::Mode::RandomPerEmitter));
    mRandomSeedMode.addItem("Random Per EmitterSet", static_cast<s32>(PtclSeed::Mode::RandomPerSet));
    mRandomSeedMode.addItem("Constant Seed", static_cast<s32>(PtclSeed::Mode::ConstantSeed));

    mRandomSeedSpinBox.setDisabled(false);
    mRandomSeedSpinBox.setRange(1, 0xFFFFFFFE);

    mFollowTypeComboBox.setEnumText(Ptcl::FollowType::All, "Follow emitter exactly");
    mFollowTypeComboBox.setEnumText(Ptcl::FollowType::PosOnly, "Follow emitter position");
    mFollowTypeComboBox.setEnumText(Ptcl::FollowType::None, "Follow disabled");

    mShapeComboBox.addItem("Particle", QVariant::fromValue(ShapeType::Particle));
    mShapeComboBox.addItem("Stripe", QVariant::fromValue(ShapeType::Stripe));
    mShapeComboBox.addItem("Primitive", QVariant::fromValue(ShapeType::Primitive));

    mMainLayout = new QFormLayout(this);

    mMainLayout->addRow("Emitter Name:", &mNameLineEdit);
    mMainLayout->addRow("Emitter Type:", &mTypeComboBox);

    auto* randomSeedLayout = new QHBoxLayout;
    randomSeedLayout->addWidget(&mRandomSeedMode);
    randomSeedLayout->addWidget(&mRandomSeedSpinBox);
    randomSeedLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->addRow("Random Seed:", randomSeedLayout);

    mMainLayout->addRow("Follow Mode:", &mFollowTypeComboBox);
    mMainLayout->addRow("Shape:", &mShapeComboBox);
    mMainLayout->addRow("Billboard Type:", &mBillboardComboBox);
    mMainLayout->addRow("Stripe Type:", &mStripeTypeComboBox);

    setupConnections();
}

void EmitterWidget::BasicPropertiesWidget::setupConnections() {
    // Emitter Name
    connect(&mNameLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        mProps.name = text;
        emit propertiesUpdated(mProps);
        emit emitterNameChanged();
    });

    // Emitter Type
    connect(&mTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.type = mTypeComboBox.currentEnum();

        if (mProps.type == Ptcl::EmitterType::Simple) {
            if (!isBillboardAllowedForShape(mProps.billboardType, ShapeType::Particle)) {
                applyBillboardType(Ptcl::BillboardType::Billboard);
                syncBillboardSelection();
            }
        }

        updateShapeRowVisibility();
        updateBillboardRowVisibility();
        emit propertiesUpdated(mProps);
        emit emitterTypeChanged();
    });

    // Random Seed Mode
    connect(&mRandomSeedMode, &QComboBox::currentIndexChanged, this, [this]() {
        auto& seed = mProps.randomSeed;
        auto mode = static_cast<PtclSeed::Mode>(mRandomSeedMode.currentData().toUInt());

        seed.setMode(mode);
        mRandomSeedSpinBox.setEnabled(mode == PtclSeed::Mode::ConstantSeed);
        emit propertiesUpdated(mProps);
    });

    // Random Seed
    connect(&mRandomSeedSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](int value) {
        auto& seed = mProps.randomSeed;
        if (seed.mode() == PtclSeed::Mode::ConstantSeed) {
            seed.setConstantSeed(static_cast<u32>(value));
        }
        emit propertiesUpdated(mProps);
    });

    // Follow Type
    connect(&mFollowTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mFollowTypeComboBox.currentEnum();

        mProps.followType = type;
        // TODO: Check if this should also be set for Ptcl::FollowType::PosOnly
        mProps.isFollow = (type == Ptcl::FollowType::All);
        emit propertiesUpdated(mProps);
    });

    // Billboard Type
    connect(&mBillboardComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        applyBillboardType(mBillboardComboBox.currentData().value<Ptcl::BillboardType>());
        emit propertiesUpdated(mProps);
    });

    // Shape Type
    connect(&mShapeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mShapeComboBox.currentData().value<ShapeType>();

        if (type == ShapeType::Particle) {
            applyBillboardType(Ptcl::BillboardType::Billboard);
        } else if (type == ShapeType::Primitive) {
            applyBillboardType(Ptcl::BillboardType::Primitive);
        } else {
            applyBillboardType(Ptcl::BillboardType::Stripe);
        }

        syncBillboardSelection();
        updateBillboardRowVisibility();
        updateShapeRowVisibility();
        emit propertiesUpdated(mProps);
    });

    // Stripe Type
    connect(&mStripeTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        applyBillboardType(mStripeTypeComboBox.currentData().value<Ptcl::BillboardType>());
        emit propertiesUpdated(mProps);
    });
}

void EmitterWidget::BasicPropertiesWidget::applyBillboardType(Ptcl::BillboardType type) {
    mProps.billboardType = type;
    mProps.isPolygon = (type == Ptcl::BillboardType::PolygonXY || type == Ptcl::BillboardType::PolygonXZ);
    mProps.isVelLook = (type == Ptcl::BillboardType::VelLook || type == Ptcl::BillboardType::VelLookPolygon);
    mProps.isEmitterBillboardMtx = mProps.isVelLook;
}

bool EmitterWidget::BasicPropertiesWidget::isBillboardAllowedForShape(Ptcl::BillboardType billboard, ShapeType shape) {
    switch (shape) {
    case ShapeType::Particle:
        return billboard != Ptcl::BillboardType::Stripe &&
               billboard != Ptcl::BillboardType::ComplexStripe &&
               billboard != Ptcl::BillboardType::Primitive;
    case ShapeType::Stripe:
        return billboard == Ptcl::BillboardType::Stripe ||
               billboard == Ptcl::BillboardType::ComplexStripe;
    case ShapeType::Primitive:
        return billboard == Ptcl::BillboardType::Primitive;
    }
    return false;
}

EmitterWidget::BasicPropertiesWidget::ShapeType EmitterWidget::BasicPropertiesWidget::shapeFromBillboard(Ptcl::BillboardType type) {
    if (type == Ptcl::BillboardType::Stripe || type == Ptcl::BillboardType::ComplexStripe) {
        return ShapeType::Stripe;
    }

    if (type == Ptcl::BillboardType::Primitive) {
        return ShapeType::Primitive;
    }

    return ShapeType::Particle;
}

void EmitterWidget::BasicPropertiesWidget::setProperties(const Ptcl::Emitter::BasicProperties& properties) {
    QSignalBlocker b1(mNameLineEdit);
    QSignalBlocker b2(mTypeComboBox);
    QSignalBlocker b3(mRandomSeedMode);
    QSignalBlocker b4(mRandomSeedSpinBox);
    QSignalBlocker b5(mFollowTypeComboBox);
    QSignalBlocker b6(mBillboardComboBox);
    QSignalBlocker b7(mShapeComboBox);
    QSignalBlocker b8(mStripeTypeComboBox);

    mProps = properties;

    mNameLineEdit.setText(mProps.name);
    mTypeComboBox.setCurrentEnum(mProps.type);

    auto& randomSeed = mProps.randomSeed;
    auto seedMode = randomSeed.mode();
    int index = mRandomSeedMode.findData(static_cast<int>(seedMode));
    if (index != -1) {
        mRandomSeedMode.setCurrentIndex(index);
    }
    mRandomSeedSpinBox.setValue(randomSeed.constantSeed());
    mRandomSeedSpinBox.setEnabled(seedMode == PtclSeed::Mode::ConstantSeed);

    mFollowTypeComboBox.setCurrentEnum(mProps.followType);

    const auto shapeType = shapeFromBillboard(mProps.billboardType);
    const s32 shapeIndex = mShapeComboBox.findData(QVariant::fromValue(shapeType));
    mShapeComboBox.setCurrentIndex(shapeIndex);

    syncBillboardSelection();
    updateBillboardRowVisibility();
    updateShapeRowVisibility();
}

void EmitterWidget::BasicPropertiesWidget::updateShapeRowVisibility() {
    if (mProps.type == Ptcl::EmitterType::Simple) {
        mMainLayout->setRowVisible(&mShapeComboBox, false);
    } else {
        mMainLayout->setRowVisible(&mShapeComboBox, true);
    }
}

void EmitterWidget::BasicPropertiesWidget::updateBillboardRowVisibility() {
    const auto shape = mShapeComboBox.currentData().value<ShapeType>();
    mMainLayout->setRowVisible(&mBillboardComboBox, shape == ShapeType::Particle);
    mMainLayout->setRowVisible(&mStripeTypeComboBox, shape == ShapeType::Stripe);
}

void EmitterWidget::BasicPropertiesWidget::syncBillboardSelection() {
    QSignalBlocker b1(mBillboardComboBox);
    QSignalBlocker b2(mStripeTypeComboBox);

    const QVariant value = QVariant::fromValue(mProps.billboardType);

    const s32 billboardIndex = mBillboardComboBox.findData(value);
    if (billboardIndex != -1) {
        mBillboardComboBox.setCurrentIndex(billboardIndex);
    }

    const s32 stripeIndex = mStripeTypeComboBox.findData(value);
    if (stripeIndex != -1) {
        mStripeTypeComboBox.setCurrentIndex(stripeIndex);
    }
}

// ========================================================================== //


} // namespace PtclEditor
