#include "editor/inspector/shapeInspector.h"

#include "math/util.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


static const std::array volumeTypeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Point,         "Point",              "Emits at a single point (origin)." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Circle,        "Circle",             "Emits on the perimeter of a circle." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::CircleSameDiv, "Circle (Divisions)", "Emits at evenly-spaced positions around a circle. Count set by Divisions." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::CircleFill,    "Circle (Filled)",    "Emits randomly within a filled circle area." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Sphere,        "Sphere",             "Emits on the surface of a sphere." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::SphereSameDiv, "Sphere (Divisions)", "Emits at evenly-spaced positions on a sphere. Count set by Divisions." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::SphereFill,    "Sphere (Filled)",    "Emits randomly within a filled sphere volume." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Cylinder,      "Cylinder",           "Emits on the surface of a cylinder. X/Z = radius, Y = height." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::CylinderFill,  "Cylinder (Filled)",  "Emits randomly within a filled cylinder." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Box,           "Box",                "Emits on the surface of a box. X/Y/Z = half-sizes." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::BoxFill,       "Box (Filled)",       "Emits randomly within a filled box." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Line,          "Line",               "Emits along a line along the Z axis." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::LineSameDiv,   "Line (Divisions)",   "Emits at evenly-spaced positions along a line. Count set by Divisions." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Rectangle,     "Rectangle",          "Emits on the perimeter of a rectangle. X/Z = half-sizes, Y unused." },
};

ShapeInspector::ShapeInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    setupUi();
    setupConnections();
}

void ShapeInspector::setupUi() {
    mRadiusSpinBox.setOrientation(Qt::Vertical);
    mRadiusSpinBox.setRange(-10000.0f, 10000.0f);
    mRadiusSpinBox.setDecimals(2);
    mRadiusSpinBox.setToolTip("Radius (or half-size for boxes/rectangles) of the emission volume.\nEach axis is independently scalable.");

    mArcWidthSpinBox.setRange(-360.0, 360.0);
    mArcWidthSpinBox.setDecimals(2);
    mArcWidthSpinBox.setSuffix(" °");
    mArcWidthSpinBox.setToolTip("Angular range of the arc in degrees.\nParticles are emitted randomly within this range starting from the Arc Start angle.\nFor SameDiv shapes, this is the angular step between divisions.");

    mArcStartSpinBox.setRange(-360.0, 360.0);
    mArcStartSpinBox.setDecimals(2);
    mArcStartSpinBox.setSuffix(" °");
    mArcStartSpinBox.setToolTip("Starting angle of the emission arc in degrees.\nCombined with Arc Width to define the full sweep range.");

    mLengthSpinBox.setRange(0.0, 10000.0);
    mLengthSpinBox.setDecimals(2);
    mLengthSpinBox.setToolTip("Length of the line along the Z axis.");

    mVolumeTblIndexComboBox.addItems({ "2", "3", "4", "6", "8", "12", "20", "32" });
    mVolumeTblIndexComboBox.setToolTip("Number of evenly-spaced particle positions around the shape.");

    mVolumeTypeComboBox.setOptions(volumeTypeOptions);
    mVolumeTypeComboBox.setDescription("Determines the shape of the volume from which particles are emitted.");

    mTranslationSpinBox.setRange(-10000.0f, 10000.0f);
    mTranslationSpinBox.setDecimals(2);
    mTranslationSpinBox.setToolTip("Offset of the emitter from the emitter set origin.\nApplied after rotation.");

    mRotationSpinBox.setRange(-360.0f, 360.0f);
    mRotationSpinBox.setDecimals(2);
    mRotationSpinBox.setToolTip("Euler rotation of the emitter in degrees (X, Y, Z axes).");

    mScaleSpinBox.setRange(-10000.0f, 10000.0f);
    mScaleSpinBox.setDecimals(2);
    mScaleSpinBox.setToolTip("Scale multiplier for the emitter volume.\nDefault (1, 1, 1) means no scaling.");

    // Layout
    auto* mainLayout = new QFormLayout;
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    setLayout(mainLayout);

    // Volume section
    addSectionHeader(mainLayout, "Volume", this);

    mVolumeFields = {
        // Volume Type
        {
            .widget     = &mVolumeTypeComboBox,
            .label      = [](Ptcl::VolumeType) { return "Shape:"; },
            .isVisible  = [](Ptcl::VolumeType) { return true; }
        },
        // Volume Table Index
        {
            .widget     = &mVolumeTblIndexComboBox,
            .label      = [](Ptcl::VolumeType) { return "Divisions:"; },
            .isVisible  = [](Ptcl::VolumeType t) {
                return t == Ptcl::VolumeType::CircleSameDiv || t == Ptcl::VolumeType::SphereSameDiv;
            }
        },
        // Radius
        {
            .widget    = &mRadiusSpinBox,
            .label     = [](Ptcl::VolumeType t) {
                if (t == Ptcl::VolumeType::Box || t == Ptcl::VolumeType::BoxFill ||
                    t == Ptcl::VolumeType::Rectangle) {
                    return "Half Size:";
                }
                return "Radius:";
            },
            .isVisible  = [](Ptcl::VolumeType t) {
                return t != Ptcl::VolumeType::Point && t != Ptcl::VolumeType::Line &&
                       t != Ptcl::VolumeType::LineSameDiv;
            }
        },
        // Length
        {
            .widget    = &mLengthSpinBox,
            .label     = [](Ptcl::VolumeType) { return "Length:"; },
            .isVisible  = [](Ptcl::VolumeType t) {
                return t == Ptcl::VolumeType::Line || t == Ptcl::VolumeType::LineSameDiv;
            }
        },
        // Sweep Width (volumeArcWidth)
        {
            .widget     = &mArcWidthSpinBox,
            .label      = [](Ptcl::VolumeType) { return "Arc Width:"; },
            .isVisible  = [](Ptcl::VolumeType t) {
                return t != Ptcl::VolumeType::Point && t != Ptcl::VolumeType::Box &&
                       t != Ptcl::VolumeType::BoxFill && t != Ptcl::VolumeType::Line &&
                       t != Ptcl::VolumeType::LineSameDiv && t != Ptcl::VolumeType::Rectangle;
            }
        },
        // Sweep Start (volumeArcStart)
        {
            .widget     = &mArcStartSpinBox,
            .label      = [](Ptcl::VolumeType) { return "Arc Start:"; },
            .isVisible  = [](Ptcl::VolumeType t) {
                return t != Ptcl::VolumeType::Point && t != Ptcl::VolumeType::Box &&
                       t != Ptcl::VolumeType::BoxFill && t != Ptcl::VolumeType::Line &&
                       t != Ptcl::VolumeType::LineSameDiv && t != Ptcl::VolumeType::Rectangle;
            }
        }
    };

    auto initialType = mVolumeTypeComboBox.currentData().value<Ptcl::VolumeType>();

    for (auto& field : mVolumeFields) {
        auto* label = new QLabel(field.label(initialType), this);
        mVolumeFieldLabels[field.widget] = label;
        mainLayout->addRow(label, field.widget);
    }

    // Transform section
    addSectionHeader(mainLayout, "Transform", this);

    mainLayout->addRow("Position:", &mTranslationSpinBox);
    mainLayout->addRow("Rotation:", &mRotationSpinBox);
    mainLayout->addRow("Scale:", &mScaleSpinBox);
}

void ShapeInspector::setupConnections() {
    // Volume connections
    connect(&mVolumeTypeComboBox, QOverload<s32>::of(&QComboBox::currentIndexChanged), this, [this]() {
        const auto type = mVolumeTypeComboBox.currentEnum();

        setEmitterProperty(
            "Set Volume Type",
            "SetVolumeType",
            &Ptcl::Emitter::volumeType,
            &Ptcl::Emitter::setVolumeType,
            type
        );
    });

    connect(&mVolumeTblIndexComboBox, QOverload<s32>::of(&QComboBox::currentIndexChanged), this, [this](s32 index) {
        setEmitterProperty(
            "Set Volume Table Index",
            "SetVolumeTblIdx",
            &Ptcl::Emitter::volumeTblIndex,
            &Ptcl::Emitter::setVolumeTblIndex,
            index
        );
    });

    connect(&mRadiusSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto radius = mRadiusSpinBox.getVector();

        setEmitterProperty(
            "Set Volume Radius",
            "SetVolumeRadius",
            &Ptcl::Emitter::volumeRadius,
            &Ptcl::Emitter::setVolumeRadius,
            radius
        );
    });

    connect(&mLengthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](f64 value) {
        auto radius = mRadiusSpinBox.getVector();
        radius.setZ(static_cast<f32>(value));

        setEmitterProperty(
            "Set Volume Length",
            "SetVolumeLength",
            &Ptcl::Emitter::volumeRadius,
            &Ptcl::Emitter::setVolumeRadius,
            radius
        );
    });

    connect(&mArcWidthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](f64 value) {
        const auto sweepStart = Math::Util::deg2idx(Math::Util::to180(static_cast<f32>(value)));

        setEmitterProperty(
            "Set Arc Width",
            "SetArcWidth",
            &Ptcl::Emitter::volumeArcWidth,
            &Ptcl::Emitter::setVolumeArcWidth,
            sweepStart
        );
    });

    connect(&mArcStartSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](f64 value) {
        const auto sweepParam = Math::Util::deg2idx(Math::Util::to180(static_cast<f32>(value)));

        setEmitterProperty(
            "Set Arc Start",
            "SetArcStart",
            &Ptcl::Emitter::volumeArcStart,
            &Ptcl::Emitter::setVolumeArcStart,
            sweepParam
        );
    });

    // Transform connections
    connect(&mTranslationSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto translation = mTranslationSpinBox.getVector();
        setEmitterProperty(
            "Set Position",
            "SetPosition",
            &Ptcl::Emitter::translation,
            &Ptcl::Emitter::setTranslation,
            translation
        );
    });

    connect(&mRotationSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto rotation = mRotationSpinBox.getVector();
        setEmitterProperty(
            "Set Rotation",
            "SetRotation",
            &Ptcl::Emitter::rotation,
            &Ptcl::Emitter::setRotation,
            rotation
        );
    });

    connect(&mScaleSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        const auto scale = mScaleSpinBox.getVector();
        setEmitterProperty(
            "Set Scale",
            "SetScale",
            &Ptcl::Emitter::scale,
            &Ptcl::Emitter::setScale,
            scale
        );
    });
}

void ShapeInspector::populateProperties() {
    QSignalBlocker b1(mVolumeTblIndexComboBox);
    QSignalBlocker b2(mVolumeTypeComboBox);
    QSignalBlocker b3(mRadiusSpinBox);
    QSignalBlocker b4(mLengthSpinBox);
    QSignalBlocker b5(mArcWidthSpinBox);
    QSignalBlocker b6(mArcStartSpinBox);
    QSignalBlocker b7(mTranslationSpinBox);
    QSignalBlocker b8(mRotationSpinBox);
    QSignalBlocker b9(mScaleSpinBox);

    auto volumeType = mEmitter->volumeType();

    mVolumeTblIndexComboBox.setCurrentIndex(mEmitter->volumeTblIndex());
    mVolumeTypeComboBox.setCurrentEnum(volumeType);

    auto& radius = mEmitter->volumeRadius();
    mRadiusSpinBox.setVector(radius);
    mLengthSpinBox.setValue(radius.getZ());

    mArcWidthSpinBox.setValue(Math::Util::to360(Math::Util::idx2deg(mEmitter->volumeArcWidth())));
    mArcStartSpinBox.setValue(Math::Util::to360(Math::Util::idx2deg(mEmitter->volumeArcStart())));

    updateVolumeFieldVisibility(volumeType);

    mTranslationSpinBox.setVector(mEmitter->translation());
    mRotationSpinBox.setVector(mEmitter->rotation());
    mScaleSpinBox.setVector(mEmitter->scale());
}

void ShapeInspector::updateVolumeFieldVisibility(Ptcl::VolumeType type) {
    for (const auto& field : mVolumeFields) {
        bool visible = field.isVisible(type);
        QWidget* widget = field.widget;
        QLabel* label = mVolumeFieldLabels[widget];
        widget->setVisible(visible);
        label->setVisible(visible);
        if (visible) {
            label->setText(field.label(type));
        }
    }
}


// ========================================================================== //


} // namespace PtclEditor
