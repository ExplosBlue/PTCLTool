#include "editor/fieldEditor/collisionDataWidget.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


FieldEditorWidget::CollisionDataWidget::CollisionDataWidget(QWidget* parent) :
    QWidget{parent} {

    // TODO: Better ranges?
    mIsWorldCheckBox.setText("Collision in world coordinates");
    mCoordSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mCoefSpinBox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mEnabledCheckBox.setText("Enabled");

    mControlsWidget = new QWidget(this);
    auto* controlsLayout = new QFormLayout(mControlsWidget);
    controlsLayout->addRow("Collision Behavior:", &mCollisionTypeSpinBox);
    controlsLayout->addRow("Coordinate System:", &mIsWorldCheckBox);
    controlsLayout->addRow("Plane Y-Coord:", &mCoordSpinBox);
    controlsLayout->addRow("Bounce Rate:", &mCoefSpinBox);

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("Collision:", &mEnabledCheckBox);
    mainLayout->addRow(mControlsWidget);

    setupConnections();
}

void FieldEditorWidget::CollisionDataWidget::setupConnections() {
    // Is Enabled
    connect(&mEnabledCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        emit isEnabledUpdated(checked);
        mControlsWidget->setEnabled(checked);
    });

    // CollisionType
    connect(&mCollisionTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        mData.collisionType = mCollisionTypeSpinBox.currentEnum();
        emit dataUpdated(mData);
    });

    // Is World
    connect(&mIsWorldCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mData.collisionIsWorld = checked;
        emit dataUpdated(mData);
    });

    // Coef
    connect(&mCoefSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mData.collisionCoef = static_cast<f32>(value);
        emit dataUpdated(mData);
    });

    // Coord
    connect(&mCoordSpinBox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mData.collisionCoord = static_cast<f32>(value);
        emit dataUpdated(mData);
    });
}

void FieldEditorWidget::CollisionDataWidget::setData(const Ptcl::FieldData::FieldCollisionData& data, bool isEnabled) {
    QSignalBlocker b1(mCollisionTypeSpinBox);
    QSignalBlocker b2(mIsWorldCheckBox);
    QSignalBlocker b3(mCoefSpinBox);
    QSignalBlocker b4(mCoordSpinBox);
    QSignalBlocker b5(mEnabledCheckBox);

    mData = data;

    mCollisionTypeSpinBox.setCurrentEnum(mData.collisionType);
    mIsWorldCheckBox.setChecked(mData.collisionIsWorld);
    mCoefSpinBox.setValue(mData.collisionCoef);
    mCoordSpinBox.setValue(mData.collisionCoord);
    mEnabledCheckBox.setChecked(isEnabled);
    mControlsWidget->setEnabled(isEnabled);
}


// ========================================================================== //


} // namespace PtclEditor
