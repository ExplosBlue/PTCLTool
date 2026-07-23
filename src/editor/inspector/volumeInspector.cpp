#include "editor/inspector/volumeInspector.h"

#include "math/util.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


static const std::array volumeTypeOptions{ // NOLINT(cert-err58-cpp)
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Point,          "Point",          "A single point volume." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Circle,         "Circle",         "A circle volume with wireframe divisions." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::CircleSameDiv,  "Circle SameDiv", "A circle with user-defined divisions." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::CircleFill,     "Circle Fill",    "A filled circle volume." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Sphere,         "Sphere",         "A sphere volume with wireframe divisions." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::SphereSameDiv,  "Sphere SameDiv", "A sphere with user-defined divisions." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::SphereFill,     "Sphere Fill",    "A filled sphere volume." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Cylinder,       "Cylinder",       "A cylinder volume." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::CylinderFill,   "Cylinder Fill",  "A filled cylinder volume." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Box,            "Box",            "A box volume." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::BoxFill,        "Box Fill",       "A filled box volume." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Line,           "Line",           "A line volume." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::LineSameDiv,    "Line SameDiv",   "A line with user-defined divisions." },
    EnumOption<Ptcl::VolumeType>{ Ptcl::VolumeType::Rectangle,      "Rectangle",      "A rectangle volume." },
};


VolumeInspector::VolumeInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    setupUi();
    setupConnections();
}

void VolumeInspector::setupUi() {
    mRadiusSpinBox.setOrientation(Qt::Vertical);

    mSweepStartSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mSweepStartSpinBox.setDecimals(4);

    mSweepParamSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mSweepParamSpinBox.setDecimals(4);

    mLengthSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mLengthSpinBox.setDecimals(4);

    mVolumeTblIndexComboBox.addItems({ "2", "3", "4", "6", "8", "12", "20", "32" });
    mTypeComboBox.setOptions(volumeTypeOptions);

    // Layout
    auto* mainLayout = new QFormLayout;
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    setLayout(mainLayout);

    // Field properties
    mFields = {
        // Volume Type
        {
            .widget     = &mTypeComboBox,
            .label      = [](Ptcl::VolumeType) { return "Volume Type:"; },
            .isVisible  = [](Ptcl::VolumeType) { return true; }
        },
        // Volume Table Index
        {
            .widget     = &mVolumeTblIndexComboBox,
            .label      = [](Ptcl::VolumeType) { return "Num Divisions:"; },
            .isVisible  = [](Ptcl::VolumeType t) {
                return t == Ptcl::VolumeType::CircleSameDiv || t == Ptcl::VolumeType::SphereSameDiv;
            }
        },
        // Radius
        {
            .widget    = &mRadiusSpinBox,
            .label     = [](Ptcl::VolumeType t) {
                if (t == Ptcl::VolumeType::Box || t == Ptcl::VolumeType::BoxFill) {
                    return "Side Length:";
                }
                return "Volume Radius:";
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
        // Sweep Start
        {
            .widget     = &mSweepStartSpinBox,
            .label      = [](Ptcl::VolumeType) { return "Arc Start Point:"; },
            .isVisible  = [](Ptcl::VolumeType t) {
                return t != Ptcl::VolumeType::Point && t != Ptcl::VolumeType::Box &&
                       t != Ptcl::VolumeType::BoxFill && t != Ptcl::VolumeType::Line &&
                       t != Ptcl::VolumeType::LineSameDiv && t != Ptcl::VolumeType::Rectangle;
            }
        },
        // Sweep Param
        {
            .widget     = &mSweepParamSpinBox,
            .label      = [](Ptcl::VolumeType) { return "Arc Width:"; },
            .isVisible  = [](Ptcl::VolumeType t) {
                return t != Ptcl::VolumeType::Point && t != Ptcl::VolumeType::Box &&
                       t != Ptcl::VolumeType::BoxFill && t != Ptcl::VolumeType::Line &&
                       t != Ptcl::VolumeType::LineSameDiv && t != Ptcl::VolumeType::Rectangle;
            }
        }
    };

    auto initialType = mTypeComboBox.currentData().value<Ptcl::VolumeType>();

    // Create label–widget pairs
    for (auto& field : mFields) {
        auto* label = new QLabel(field.label(initialType), this);
        mFieldLabels[field.widget] = label;
        mainLayout->addRow(label, field.widget);
    }
}

void VolumeInspector::setupConnections() {
    connect(&mTypeComboBox, QOverload<s32>::of(&QComboBox::currentIndexChanged), this, [this]() {
        const auto type = mTypeComboBox.currentEnum();

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

    connect(&mSweepStartSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](f64 value) {
        const auto sweepStart = Math::Util::deg2idx(Math::Util::to180(static_cast<f32>(value)));

        setEmitterProperty(
            "Set Sweep Start",
            "SetSweepStart",
            &Ptcl::Emitter::volumeArcWidth,
            &Ptcl::Emitter::setVolumeArcWidth,
            sweepStart
        );
    });

    connect(&mSweepParamSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](f64 value) {
        const auto sweepParam = Math::Util::deg2idx(Math::Util::to180(static_cast<f32>(value)));

        setEmitterProperty(
            "Set Sweep Param",
            "SetSweepParam",
            &Ptcl::Emitter::volumeArcStart,
            &Ptcl::Emitter::setVolumeArcStart,
            sweepParam
        );
    });
}

void VolumeInspector::populateProperties() {
    QSignalBlocker b1(mVolumeTblIndexComboBox);
    QSignalBlocker b2(mTypeComboBox);
    QSignalBlocker b3(mRadiusSpinBox);
    QSignalBlocker b4(mLengthSpinBox);
    QSignalBlocker b5(mSweepStartSpinBox);
    QSignalBlocker b6(mSweepParamSpinBox);

    auto volumeType = mEmitter->volumeType();

    mVolumeTblIndexComboBox.setCurrentIndex(mEmitter->volumeTblIndex());
    mTypeComboBox.setCurrentEnum(volumeType);

    auto& radius = mEmitter->volumeRadius();
    mRadiusSpinBox.setVector(radius);
    mLengthSpinBox.setValue(radius.getZ());

    mSweepStartSpinBox.setValue(Math::Util::to360(Math::Util::idx2deg(mEmitter->volumeArcWidth())));
    mSweepParamSpinBox.setValue(Math::Util::to360(Math::Util::idx2deg(mEmitter->volumeArcStart())));

    updateFieldVisibility(volumeType);
}

void VolumeInspector::updateFieldVisibility(Ptcl::VolumeType type) {
    for (const auto& field : mFields) {
        bool visible = field.isVisible(type);
        QWidget* widget = field.widget;
        QLabel* label = mFieldLabels[widget];
        widget->setVisible(visible);
        label->setVisible(visible);
        if (visible) {
            label->setText(field.label(type));
        }
    }
}


// ========================================================================== //


} // namespace PtclEditor
