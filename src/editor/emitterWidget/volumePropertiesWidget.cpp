#include "editor/emitterWidget/volumePropertiesWidget.h"

#include "math/util.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


VolumePropertiesWidget::VolumePropertiesWidget(QWidget* parent) :
    EmitterWidgetBase{parent} {

    setupUi();
    setupConnections();
}

void VolumePropertiesWidget::setupUi() {
    mRadiusSpinBox.setOrientation(Qt::Vertical);

    mSweepStartSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mSweepStartSpinBox.setDecimals(4);

    mSweepParamSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mSweepParamSpinBox.setDecimals(4);

    mLengthSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mLengthSpinBox.setDecimals(4);

    mVolumeTblIndexComboBox.addItems({ "2", "3", "4", "6", "8", "12", "20", "32" });

    // Layout
    auto* mainLayout = new QFormLayout;
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
            .label     = [](Ptcl::VolumeType t) { return "Length:"; },
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

void VolumePropertiesWidget::setupConnections() {
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
            &Ptcl::Emitter::volumeSweepStart,
            &Ptcl::Emitter::setVolumeSweepStart,
            sweepStart
        );
    });

    connect(&mSweepParamSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](f64 value) {
        const auto sweepParam = Math::Util::deg2idx(Math::Util::to180(static_cast<f32>(value)));

        setEmitterProperty(
            "Set Sweep Param",
            "SetSweepParam",
            &Ptcl::Emitter::volumeSweepParam,
            &Ptcl::Emitter::setVolumeSweepParam,
            sweepParam
        );
    });
}

void VolumePropertiesWidget::populateProperties() {
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

    mSweepStartSpinBox.setValue(Math::Util::to360(Math::Util::idx2deg(mEmitter->volumeSweepStart())));
    mSweepParamSpinBox.setValue(Math::Util::to360(Math::Util::idx2deg(mEmitter->volumeSweepParam())));

    updateFieldVisibility(volumeType);
}

void VolumePropertiesWidget::updateFieldVisibility(Ptcl::VolumeType type) {
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
