#include "editor/volumePropertiesWidget.h"
#include "util/mathUtil.h"

#include <QFormLayout>

// ========================================================================== //


VolumePropertiesWidget::VolumePropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    mRadiusSpinBox.setOrientation(Qt::Vertical);

    mSweepStartSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mSweepStartSpinBox.setDecimals(4);

    mSweepParamSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mSweepParamSpinBox.setDecimals(4);

    mLengthSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mLengthSpinBox.setDecimals(4);

    mVolumeTblIndexComboBox.addItems({ "2", "3", "4", "6", "8", "12", "20", "32" });

    // Layout
    QFormLayout* formLayout = new QFormLayout;

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
        formLayout->addRow(label, field.widget);
    }

    setLayout(formLayout);

    // Signals
    connect(&mTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        if (!mEmitterPtr || misPopulating) return;
        auto type = mTypeComboBox.currentEnum();
        mEmitterPtr->setVolumeType(type);
        updateFieldVisibility(type);
    });

    connect(&mVolumeTblIndexComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        if (!mEmitterPtr || misPopulating) return;
        mEmitterPtr->setVolumeTblIndex(index);
    });

    connect(&mRadiusSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        if (!mEmitterPtr || misPopulating) return;

        auto radius = mRadiusSpinBox.getVector();

        mEmitterPtr->setVolumeRadius(radius);
        // Keep length spinbox in sync
        const QSignalBlocker block(&mLengthSpinBox);
        mLengthSpinBox.setValue(radius.z());

    });

    connect(&mLengthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
        if (!mEmitterPtr || misPopulating) return;

        auto radius = mRadiusSpinBox.getVector();
        radius.setZ(value);

        mEmitterPtr->setVolumeRadius(radius);
        // Keep radius spinbox in sync
        const QSignalBlocker block(&mRadiusSpinBox);
        mRadiusSpinBox.setVector(radius);
    });

    connect(&mSweepStartSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
        if (!mEmitterPtr || misPopulating) return;
        mEmitterPtr->setVolumeSweepStart(MathUtil::deg2idx(MathUtil::to180(value)));
    });

    connect(&mSweepParamSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
        if (!mEmitterPtr || misPopulating) return;
        mEmitterPtr->setVolumeSweepParam(MathUtil::deg2idx(MathUtil::to180(value)));
    });
}

void VolumePropertiesWidget::setEmitter(Ptcl::Emitter* emitter) {
    mEmitterPtr = emitter;
    populateWidgets();
}

void VolumePropertiesWidget::populateWidgets() {
    if (!mEmitterPtr) {
        return;
    }

    misPopulating = true;

    auto volumeType = mEmitterPtr->volumeType();

    mVolumeTblIndexComboBox.setCurrentIndex(mEmitterPtr->volumeTblIndex());
    mTypeComboBox.setCurrentEnum(volumeType);

    auto& radius = mEmitterPtr->volumeRadius();
    mRadiusSpinBox.setVector(radius);
    mLengthSpinBox.setValue(radius.z());

    mSweepStartSpinBox.setValue(MathUtil::to360(MathUtil::idx2deg(mEmitterPtr->volumeSweepStart())));
    mSweepParamSpinBox.setValue(MathUtil::to360(MathUtil::idx2deg(mEmitterPtr->volumeSweepParam())));

    updateFieldVisibility(volumeType);

    misPopulating = false;
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
