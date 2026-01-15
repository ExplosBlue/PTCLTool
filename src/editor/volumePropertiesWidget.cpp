#include "editor/volumePropertiesWidget.h"
#include "math/util.h"

#include <QFormLayout>

// ========================================================================== //


VolumePropertiesWidget::VolumePropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    setupUi();
    setupSignals();
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

void VolumePropertiesWidget::setupSignals() {
    connect(&mTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        auto type = mTypeComboBox.currentEnum();
        mProps.volumeType = type;
        updateFieldVisibility(type);
        emit propertiesUpdated(mProps);
    });

    connect(&mVolumeTblIndexComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index) {
        mProps.volumeTblIndex = index;
        emit propertiesUpdated(mProps);
    });

    connect(&mRadiusSpinBox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        auto radius = mRadiusSpinBox.getVector();

        mProps.volumeRadius = radius;
        // Keep length spinbox in sync
        const QSignalBlocker block(&mLengthSpinBox);
        mLengthSpinBox.setValue(radius.z());
        emit propertiesUpdated(mProps);
    });

    connect(&mLengthSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
        auto radius = mRadiusSpinBox.getVector();
        radius.setZ(static_cast<f32>(value));

        mProps.volumeRadius = radius;
        // Keep radius spinbox in sync
        const QSignalBlocker block(&mRadiusSpinBox);
        mRadiusSpinBox.setVector(radius);
        emit propertiesUpdated(mProps);
    });

    connect(&mSweepStartSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
        mProps.volumeSweepStart = Math::Util::deg2idx(Math::Util::to180(static_cast<f32>(value)));
        emit propertiesUpdated(mProps);
    });

    connect(&mSweepParamSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double value) {
        mProps.volumeSweepParam = Math::Util::deg2idx(Math::Util::to180(static_cast<f32>(value)));
        emit propertiesUpdated(mProps);
    });
}

void VolumePropertiesWidget::setProperties(const Ptcl::VolumeProperties& properties) {
    mProps = properties;
    populateWidgets();
}

void VolumePropertiesWidget::populateWidgets() {
    QSignalBlocker b1(mVolumeTblIndexComboBox);
    QSignalBlocker b2(mTypeComboBox);
    QSignalBlocker b3(mRadiusSpinBox);
    QSignalBlocker b4(mLengthSpinBox);
    QSignalBlocker b5(mSweepStartSpinBox);
    QSignalBlocker b6(mSweepParamSpinBox);

    auto volumeType = mProps.volumeType;

    mVolumeTblIndexComboBox.setCurrentIndex(mProps.volumeTblIndex);
    mTypeComboBox.setCurrentEnum(volumeType);

    auto& radius = mProps.volumeRadius;
    mRadiusSpinBox.setVector(radius);
    mLengthSpinBox.setValue(radius.z());

    mSweepStartSpinBox.setValue(Math::Util::to360(Math::Util::idx2deg(mProps.volumeSweepStart)));
    mSweepParamSpinBox.setValue(Math::Util::to360(Math::Util::idx2deg(mProps.volumeSweepParam)));

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
