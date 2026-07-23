#include "editor/inspector/generalEmitterInspector.h"
#include "util/nameValidator.h"

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

namespace PtclEditor {


// ========================================================================== //


static const std::array typeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::EmitterType>{ Ptcl::EmitterType::Simple,  "Simple",  "Uses basic lifecycle." },
    EnumOption<Ptcl::EmitterType>{ Ptcl::EmitterType::Complex, "Complex", "Enables advanced effects like fluctuation." },
    EnumOption<Ptcl::EmitterType>{ Ptcl::EmitterType::Compact, "Compact", "Disables features for performance." },
};

static const std::array randomOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<PtclSeed::Mode>{ PtclSeed::Mode::RandomPerEmitter, "Random Per Emitter", "Generates a new random seed each time the emitter is instantiated." },
    EnumOption<PtclSeed::Mode>{ PtclSeed::Mode::RandomPerSet,     "Random Per Set",     "Generates a new random seed each time the emitter set is instantiated." },
    EnumOption<PtclSeed::Mode>{ PtclSeed::Mode::ConstantSeed,     "Constant",           "Uses a fixed value as a random seed (for consistent results)." }
};

static const std::array shapeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<GeneralEmitterInspector::ShapeType>{ GeneralEmitterInspector::ShapeType::Particle,  "Particle",  "Renders as a billboard/polygon." },
    EnumOption<GeneralEmitterInspector::ShapeType>{ GeneralEmitterInspector::ShapeType::Stripe,    "Ribbon",    "Renders as a connected ribbon trail. Used to create trails and flowing effects." },
    EnumOption<GeneralEmitterInspector::ShapeType>{ GeneralEmitterInspector::ShapeType::Primitive, "Primitive", "Renders as a geometric shape." },
};

static const std::array followOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::FollowType>{ Ptcl::FollowType::All,     "Exact",         "Particles inherit the position, rotation and scale of the emitter each frame." },
    EnumOption<Ptcl::FollowType>{ Ptcl::FollowType::PosOnly, "Position Only", "Particles inherit the position of the emitter each frame." },
    EnumOption<Ptcl::FollowType>{ Ptcl::FollowType::None,    "Disabled",      "Particles don't inherit any position, rotation or scale from the emitter." },
};

static const std::array stripeTypeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::StripeType>{ Ptcl::StripeType::Billboard,     "Camera Facing",    "The ribbon faces the camera, rotating to always point toward the viewer." },
    EnumOption<Ptcl::StripeType>{ Ptcl::StripeType::EmitterMatrix, "Emitter Oriented",  "The ribbon width fans out based on the emitter's orientation and the ribbon's travel direction." },
    EnumOption<Ptcl::StripeType>{ Ptcl::StripeType::EmitterUpDown, "Upward Facing",    "The ribbon always faces upward along the emitter's Y axis, regardless of travel direction." },
};

static const std::array billboardTypeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::Billboard,      "Camera Facing",         "Particle always faces the camera on all axes." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::PolygonXY,      "Fixed (XY Plane)",      "Particle is fixed flat on the XY plane (vertical)." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::PolygonXZ,      "Fixed (XZ Plane)",      "Particle is fixed flat on the XZ plane (horizontal)." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::VelLook,        "Velocity Billboard",    "Particle rotates to face along its direction of travel." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::VelLookPolygon, "Velocity Polygon",      "Particle is fixed to the velocity direction using emitter orientation." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::BillboardY,     "Camera Facing (Y Axis)", "Particle rotates to face camera, but only around the Y axis." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::Stripe,         "Trail",               "Particles form a connected ribbon trail with smooth transitions between points." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::ComplexStripe,  "Individual",          "Each particle renders as a separate billboard. No ribbon mesh is generated." },
    EnumOption<Ptcl::BillboardType>{ Ptcl::BillboardType::Primitive,      "Primitive",             "Renders a 3D geometric primitive shape." },
};


// ========================================================================== //


GeneralEmitterInspector::GeneralEmitterInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    mMainLayout = new QFormLayout(this);
    mMainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // General
    addSectionHeader(mMainLayout, "General", this);

    mNameLineEdit.setValidator(new EmitterNameValidator(&mNameLineEdit));
    mNameLineEdit.setToolTip("The name for this emitter.");
    mMainLayout->addRow("Name:", &mNameLineEdit);

    mTypeComboBox.setOptions(typeOptions);
    mTypeComboBox.setDescription("Determines the calculation complexity of the emitter.");
    mMainLayout->addRow("Emitter Type:", &mTypeComboBox);

    // Behavior
    addSectionHeader(mMainLayout, "Behavior", this);

    auto* randomSeedLayout = new QHBoxLayout;
    mRandomSeedMode.setOptions(randomOptions);
    mRandomSeedMode.setDescription("Controls randomization.");
    randomSeedLayout->addWidget(&mRandomSeedMode);

    mRandomSeedSpinBox.setDisabled(false);
    mRandomSeedSpinBox.setRange(1, 0xFFFFFFFE);
    mRandomSeedSpinBox.setToolTip("The seed used for randomization when random mode is 'Constant'");
    randomSeedLayout->addWidget(&mRandomSeedSpinBox);
    randomSeedLayout->setContentsMargins(0, 0, 0, 0);
    mMainLayout->addRow("Random Seed:", randomSeedLayout);

    mFollowTypeComboBox.setOptions(followOptions);    
    mFollowTypeComboBox.setDescription("Determines how the particle follows the emitter.");
    mMainLayout->addRow("Follow Mode:", &mFollowTypeComboBox);

    // Rendering
    addSectionHeader(mMainLayout, "Rendering", this);

    mShapeComboBox.setOptions(shapeOptions);
    mShapeComboBox.setDescription("Determines how the geometry of the emitter is constructed.");
    mMainLayout->addRow("Shape:", &mShapeComboBox);

    mBillboardTypeComboBox.setOptions(billboardTypeOptions);
    mBillboardTypeComboBox.setDescription("Determines how the emitter is oriented and rendered.");
    mMainLayout->addRow("Billboard Type:", &mBillboardTypeComboBox);

    // Stripe section
    mStripeSection = new QWidget(this);
    auto* stripeOuterLayout = new QVBoxLayout(mStripeSection);
    stripeOuterLayout->setContentsMargins(0, 0, 0, 0);
    stripeOuterLayout->setSpacing(0);

    // Stripe header
    {
        auto* headerRow = new QHBoxLayout;
        headerRow->setContentsMargins(0, 4, 0, 0);
        headerRow->setSpacing(8);
        auto* headerLabel = new QLabel("Stripe", this);
        headerLabel->setStyleSheet("font-weight: bold;");
        headerRow->addWidget(headerLabel);
        auto* headerLine = new QFrame(this);
        headerLine->setFrameShape(QFrame::HLine);
        headerLine->setFrameShadow(QFrame::Sunken);
        headerLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        headerRow->addWidget(headerLine);
        auto* headerWidget = new QWidget(this);
        headerWidget->setLayout(headerRow);
        stripeOuterLayout->addWidget(headerWidget);
    }

    // Stripe form
    auto* stripeForm = new QFormLayout;
    stripeForm->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    // Orientation
    mStripeTypeComboBox.setOptions(stripeTypeOptions);
    mStripeTypeComboBox.setDescription("Controls how the ribbon width is oriented.");
    stripeForm->addRow("Ribbon Type:", &mStripeTypeComboBox);

    mStripeEmitterCoordCheckBox.setText("Emitter Relative");
    mStripeEmitterCoordCheckBox.setToolTip("When enabled, the ribbon moves with the emitter. When disabled, the ribbon stays in world space.");
    mStripeEmitterCoordCheckBox.setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    stripeForm->addRow("Follow:", &mStripeEmitterCoordCheckBox);

    mStripeDirInterpolateSpinBox.setRange(0.0f, 1.0f);
    mStripeDirInterpolateSpinBox.setToolTip("How smoothly the ribbon turns. 0 = sharp turns, 1 = very smooth.");
    stripeForm->addRow("Direction Smoothing:", &mStripeDirInterpolateSpinBox);

    // Geometry
    mStripeNumHistSpinBox.setRange(2, 128);
    mStripeNumHistSpinBox.setToolTip("How many past positions are stored for the ribbon. Older positions are replaced when full.");
    stripeForm->addRow("History Buffer Size:", &mStripeNumHistSpinBox);

    mStripeHistStepSpinBox.setRange(0, 64);
    mStripeHistStepSpinBox.setToolTip("How many ribbon segments are generated each frame. Higher values make smoother ribbons.");
    stripeForm->addRow("Steps Per Frame:", &mStripeHistStepSpinBox);

    // Appearance
    mStripeStartAlphaSpinBox.setRange(0.0f, 1.0f);
    mStripeStartAlphaSpinBox.setSingleStep(0.1f);
    mStripeStartAlphaSpinBox.setToolTip("Opacity at the emitter end of the ribbon. Fades toward Tail Alpha along the ribbon.");
    stripeForm->addRow("Head Alpha:", &mStripeStartAlphaSpinBox);

    mStripeEndAlphaSpinBox.setRange(0.0f, 1.0f);
    mStripeEndAlphaSpinBox.setSingleStep(0.1f);
    mStripeEndAlphaSpinBox.setToolTip("Opacity at the far end of the ribbon. Fades from Head Alpha along the ribbon.");
    stripeForm->addRow("Tail Alpha:", &mStripeEndAlphaSpinBox);

    mStripeUVScrollSpeedSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mStripeUVScrollSpeedSpinBox.setToolTip("How fast the ribbon texture scrolls along its length and width each frame.");
    stripeForm->addRow("UV Scroll Speed:", &mStripeUVScrollSpeedSpinBox);

    auto* stripeFormWidget = new QWidget(this);
    stripeFormWidget->setLayout(stripeForm);
    stripeOuterLayout->addWidget(stripeFormWidget);

    mMainLayout->addRow(mStripeSection);

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
            }
        }

        stack->endMacro();

        updateShapeRowVisibility();
        updateBillboardFilter();
    });

    // Random Seed Mode
    connect(&mRandomSeedMode, &QComboBox::currentIndexChanged, this, [this]() {
        auto seed = mEmitter->randomSeed();
        auto mode = mRandomSeedMode.currentEnum();

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
    connect(&mRandomSeedSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
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
    connect(&mBillboardTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mBillboardTypeComboBox.currentEnum();
        setBillboardType(type, "Set Billboard Type", "BillboardType");
        updateStripeSectionVisibility();
    });

    // Shape Type    
    connect(&mShapeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mShapeComboBox.currentEnum();

        Ptcl::BillboardType newBillboard = Ptcl::BillboardType::Billboard;
        switch (type) {
            case ShapeType::Particle:   newBillboard = Ptcl::BillboardType::Billboard;  break;
            case ShapeType::Primitive:  newBillboard = Ptcl::BillboardType::Primitive;  break;
            case ShapeType::Stripe:     newBillboard = Ptcl::BillboardType::Stripe;     break;
        }

        setBillboardType(newBillboard, "Set Shape Type", "ShapeType");
        updateBillboardFilter();
        updateStripeSectionVisibility();
    });

    // Stripe Type
    connect(&mStripeTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        setEmitterProperty(
            "Set Stripe Type",
            "SetStripeType",
            &Ptcl::Emitter::stripeType,
            &Ptcl::Emitter::setStripeType,
            mStripeTypeComboBox.currentEnum()
        );
    });

    // Stripe Emitter Coord
    connect(&mStripeEmitterCoordCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        setEmitterProperty(
            "Toggle Stripe Follow Emitter",
            "SetStripeEmitterCoord",
            &Ptcl::Emitter::isStripeEmitterCoord,
            &Ptcl::Emitter::setStripeEmitterCoord,
            checked
        );
    });

    // Stripe Num History
    connect(&mStripeNumHistSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Stripe History Size",
            "SetStripeNumHist",
            &Ptcl::Emitter::stripeNumHistory,
            &Ptcl::Emitter::setStripeNumHistory,
            value
        );
    });

    // Stripe Start Alpha
    connect(&mStripeStartAlphaSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Stripe Start Alpha",
            "SetStripeStartAlpha",
            &Ptcl::Emitter::stripeStartAlpha,
            &Ptcl::Emitter::setStripeStartAlpha,
            static_cast<f32>(value)
        );
    });

    // Stripe End Alpha
    connect(&mStripeEndAlphaSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Stripe End Alpha",
            "SetStripeEndAlpha",
            &Ptcl::Emitter::stripeEndAlpha,
            &Ptcl::Emitter::setStripeEndAlpha,
            static_cast<f32>(value)
        );
    });

    // Stripe UV Scroll Speed
    connect(&mStripeUVScrollSpeedSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto speed = mStripeUVScrollSpeedSpinBox.getVector();
        setEmitterProperty(
            "Set Stripe UV Scroll Speed",
            "SetStripeUVSpeed",
            &Ptcl::Emitter::stripeUVScrollSpeed,
            &Ptcl::Emitter::setStripeUVScrollSpeed,
            speed
        );
    });

    // Stripe History Step
    connect(&mStripeHistStepSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setEmitterProperty(
            "Set Stripe History Spacing",
            "SetStripeHistStep",
            &Ptcl::Emitter::stripeHistoryStep,
            &Ptcl::Emitter::setStripeHistoryStep,
            value
        );
    });

    // Stripe Direction Interpolation
    connect(&mStripeDirInterpolateSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        setEmitterProperty(
            "Set Stripe Interpolation Ratio",
            "SetStripeDirInterp",
            &Ptcl::Emitter::stripeDirInterpolate,
            &Ptcl::Emitter::setStripeDirInterpolate,
            static_cast<f32>(value)
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
    QSignalBlocker b6(mBillboardTypeComboBox);
    QSignalBlocker b7(mShapeComboBox);
    QSignalBlocker b8(mStripeTypeComboBox);
    QSignalBlocker b9(mStripeEmitterCoordCheckBox);
    QSignalBlocker b10(mStripeNumHistSpinBox);
    QSignalBlocker b11(mStripeStartAlphaSpinBox);
    QSignalBlocker b12(mStripeEndAlphaSpinBox);
    QSignalBlocker b13(mStripeUVScrollSpeedSpinBox);
    QSignalBlocker b14(mStripeHistStepSpinBox);
    QSignalBlocker b15(mStripeDirInterpolateSpinBox);

    mNameLineEdit.setText(mEmitter->name());
    mTypeComboBox.setCurrentEnum(mEmitter->type());

    auto& randomSeed = mEmitter->randomSeed();
    auto seedMode = randomSeed.mode();
    mRandomSeedMode.setCurrentEnum(seedMode);
    mRandomSeedSpinBox.setValue(randomSeed.constantSeed());
    mRandomSeedSpinBox.setEnabled(seedMode == PtclSeed::Mode::ConstantSeed);

    mFollowTypeComboBox.setCurrentEnum(mEmitter->followType());

    const auto shapeType = shapeFromBillboard(mEmitter->billboardType());
    mShapeComboBox.setCurrentEnum(shapeType);
    mBillboardTypeComboBox.setCurrentEnum(mEmitter->billboardType());
    updateBillboardFilter();
    updateShapeRowVisibility();

    mStripeTypeComboBox.setCurrentEnum(mEmitter->stripeType());
    mStripeEmitterCoordCheckBox.setChecked(mEmitter->isStripeEmitterCoord());
    mStripeNumHistSpinBox.setValue(mEmitter->stripeNumHistory());
    mStripeStartAlphaSpinBox.setValue(mEmitter->stripeStartAlpha());
    mStripeEndAlphaSpinBox.setValue(mEmitter->stripeEndAlpha());
    mStripeUVScrollSpeedSpinBox.setVector(mEmitter->stripeUVScrollSpeed());
    mStripeHistStepSpinBox.setValue(mEmitter->stripeHistoryStep());
    mStripeDirInterpolateSpinBox.setValue(mEmitter->stripeDirInterpolate());
    updateStripeSectionVisibility();
}

void GeneralEmitterInspector::updateShapeRowVisibility() {
    if (mEmitter->type() == Ptcl::EmitterType::Simple) {
        mMainLayout->setRowVisible(&mShapeComboBox, false);
    } else {
        mMainLayout->setRowVisible(&mShapeComboBox, true);
    }
}

void GeneralEmitterInspector::updateBillboardFilter() {
    const auto shape = mShapeComboBox.currentEnum();
    mBillboardTypeComboBox.setFilter([shape](Ptcl::BillboardType type) {
        return isBillboardAllowedForShape(type, shape);
    });
}

void GeneralEmitterInspector::updateStripeSectionVisibility() {
    const auto shape = mShapeComboBox.currentEnum();
    const auto billboard = mBillboardTypeComboBox.currentEnum();
    const bool isStripeShape = shape == ShapeType::Stripe;
    const bool isComplexStripe = billboard == Ptcl::BillboardType::ComplexStripe;
    const bool enabled = isStripeShape && !isComplexStripe;

    mStripeSection->setVisible(isStripeShape);
    mStripeTypeComboBox.setEnabled(enabled);
    mStripeEmitterCoordCheckBox.setEnabled(enabled);
    mStripeNumHistSpinBox.setEnabled(enabled);
    mStripeStartAlphaSpinBox.setEnabled(enabled);
    mStripeEndAlphaSpinBox.setEnabled(enabled);
    mStripeUVScrollSpeedSpinBox.setEnabled(enabled);
    mStripeHistStepSpinBox.setEnabled(enabled);
    mStripeDirInterpolateSpinBox.setEnabled(enabled);
}


// ========================================================================== //


} // namespace PtclEditor
