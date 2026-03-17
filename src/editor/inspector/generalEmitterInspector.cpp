#include "editor/inspector/generalEmitterInspector.h"
#include "util/nameValidator.h"

#include <QLabel>

namespace PtclEditor {


// ========================================================================== //


GeneralEmitterInspector::GeneralEmitterInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

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
    mMainLayout->addRow("Is Billboard Matrix:", &mIsBillboardMtxCheckBox);

    setupConnections();
}

void GeneralEmitterInspector::setupConnections() {
    // Emitter Name
    connect(&mNameLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        setEmitterProperty(
            "Set Name",
            "EmitterName",
            &Ptcl::Emitter::name,
            &Ptcl::Emitter::setName,
            text
        );
    });

    // Emitter Type
    connect(&mTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        auto* stack = mDocument->undoStack();
        stack->beginMacro(formatHistoryLabel("Set Type"));

        setEmitterProperty(
            "Set Type",
            "EmitterType",
            &Ptcl::Emitter::type,
            &Ptcl::Emitter::setType,
            mTypeComboBox.currentEnum()
        );

        if (mEmitter->type() == Ptcl::EmitterType::Simple) {
            if (!isBillboardAllowedForShape(mEmitter->billboardType(), ShapeType::Particle)) {
                setBillboardType(Ptcl::BillboardType::Billboard, "Auto Fix Billboard Type", "BillboardType");
                syncBillboardSelection();
            }
        }

        stack->endMacro();

        updateShapeRowVisibility();
        updateBillboardRowVisibility();
    });

    // Random Seed Mode
    connect(&mRandomSeedMode, &QComboBox::currentIndexChanged, this, [this]() {
        auto seed = mEmitter->randomSeed();
        auto mode = static_cast<PtclSeed::Mode>(mRandomSeedMode.currentData().toUInt());

        seed.setMode(mode);

        setEmitterProperty(
            "Set Random Seed Mode",
            "RandomSeedMode",
            &Ptcl::Emitter::randomSeed,
            &Ptcl::Emitter::setRandomSeed,
            seed
        );

        mRandomSeedSpinBox.setEnabled(mode == PtclSeed::Mode::ConstantSeed);
    });

    // Random Seed
    connect(&mRandomSeedSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](int value) {
        auto seed = mEmitter->randomSeed();
        if (seed.mode() == PtclSeed::Mode::ConstantSeed) {
            seed.setConstantSeed(static_cast<u32>(value));
        }

        setEmitterProperty(
            "Set Random Seed",
            "RandomSeed",
            &Ptcl::Emitter::randomSeed,
            &Ptcl::Emitter::setRandomSeed,
            seed
        );
    });

    // Follow Type
    connect(&mFollowTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mFollowTypeComboBox.currentEnum();

        setEmitterProperty(
            "Set Follow Type",
            "FollowType",
            &Ptcl::Emitter::followType,
            &Ptcl::Emitter::setFollowType,
            type
        );
    });

    // Billboard Type
    connect(&mBillboardComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        setBillboardType(mBillboardComboBox.currentData().value<Ptcl::BillboardType>(), "Set Billboard Type", "BillboardType");
    });

    // Shape Type    
    connect(&mShapeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mShapeComboBox.currentData().value<ShapeType>();

        Ptcl::BillboardType newBillboard;
        switch (type) {
            case ShapeType::Particle:   newBillboard = Ptcl::BillboardType::Billboard;  break;
            case ShapeType::Primitive:  newBillboard = Ptcl::BillboardType::Primitive;  break;
            case ShapeType::Stripe:     newBillboard = Ptcl::BillboardType::Stripe;     break;
        }

        setBillboardType(newBillboard, "Set Shape Type", "ShapeType");
        syncBillboardSelection();
        updateBillboardRowVisibility();
        updateShapeRowVisibility();
    });

    // Stripe Type
    connect(&mStripeTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        setBillboardType(mStripeTypeComboBox.currentData().value<Ptcl::BillboardType>(), "Set Stripe Type", "StripeType");
    });

    // Is Billboard Mtx
    connect(&mIsBillboardMtxCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Set Billboard Matrix",
            "IsBillboardMtx",
            &Ptcl::Emitter::isEmitterBillboardMtx,
            &Ptcl::Emitter::setIsEmitterBillboardMtx,
            checked
        );
    });
}

void GeneralEmitterInspector::setBillboardType(Ptcl::BillboardType type, const QString& label, const QString& key) {
    setEmitterProperty(
        label,
        key,
        &Ptcl::Emitter::billboardType,
        &Ptcl::Emitter::setBillboardType,
        type
    );
}

bool GeneralEmitterInspector::isBillboardAllowedForShape(Ptcl::BillboardType billboard, ShapeType shape) {
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

GeneralEmitterInspector::ShapeType GeneralEmitterInspector::shapeFromBillboard(Ptcl::BillboardType type) {
    if (type == Ptcl::BillboardType::Stripe || type == Ptcl::BillboardType::ComplexStripe) {
        return ShapeType::Stripe;
    }

    if (type == Ptcl::BillboardType::Primitive) {
        return ShapeType::Primitive;
    }

    return ShapeType::Particle;
}

void GeneralEmitterInspector::populateProperties() {
    QSignalBlocker b1(mNameLineEdit);
    QSignalBlocker b2(mTypeComboBox);
    QSignalBlocker b3(mRandomSeedMode);
    QSignalBlocker b4(mRandomSeedSpinBox);
    QSignalBlocker b5(mFollowTypeComboBox);
    QSignalBlocker b6(mBillboardComboBox);
    QSignalBlocker b7(mShapeComboBox);
    QSignalBlocker b8(mStripeTypeComboBox);
    QSignalBlocker b9(mIsBillboardMtxCheckBox);

    mNameLineEdit.setText(mEmitter->name());
    mTypeComboBox.setCurrentEnum(mEmitter->type());

    auto& randomSeed = mEmitter->randomSeed();
    auto seedMode = randomSeed.mode();
    s32 index = mRandomSeedMode.findData(static_cast<s32>(seedMode));
    if (index != -1) {
        mRandomSeedMode.setCurrentIndex(index);
    }
    mRandomSeedSpinBox.setValue(randomSeed.constantSeed());
    mRandomSeedSpinBox.setEnabled(seedMode == PtclSeed::Mode::ConstantSeed);

    mFollowTypeComboBox.setCurrentEnum(mEmitter->followType());

    const auto shapeType = shapeFromBillboard(mEmitter->billboardType());
    const s32 shapeIndex = mShapeComboBox.findData(QVariant::fromValue(shapeType));
    mShapeComboBox.setCurrentIndex(shapeIndex);

    mIsBillboardMtxCheckBox.setChecked(mEmitter->isEmitterBillboardMtx());

    syncBillboardSelection();
    updateBillboardRowVisibility();
    updateShapeRowVisibility();
}

void GeneralEmitterInspector::updateShapeRowVisibility() {
    if (mEmitter->type() == Ptcl::EmitterType::Simple) {
        mMainLayout->setRowVisible(&mShapeComboBox, false);
    } else {
        mMainLayout->setRowVisible(&mShapeComboBox, true);
    }
}

void GeneralEmitterInspector::updateBillboardRowVisibility() {
    const auto shape = mShapeComboBox.currentData().value<ShapeType>();
    mMainLayout->setRowVisible(&mBillboardComboBox, shape == ShapeType::Particle);
    mMainLayout->setRowVisible(&mStripeTypeComboBox, shape == ShapeType::Stripe);
}

void GeneralEmitterInspector::syncBillboardSelection() {
    QSignalBlocker b1(mBillboardComboBox);
    QSignalBlocker b2(mStripeTypeComboBox);

    const QVariant value = QVariant::fromValue(mEmitter->billboardType());

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
