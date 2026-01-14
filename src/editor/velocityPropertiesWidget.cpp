#include "editor/velocityPropertiesWidget.h"

#include <QFormLayout>


// ========================================================================== //


VelocityPropertiesWidget::VelocityPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    mFigureVelSpinbox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mInitVelSpinbox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());
    mVelRandomSpinbox.setRange(std::numeric_limits<f32>::lowest(), std::numeric_limits<f32>::max());

    mainLayout->addRow("Figure Velocity:", &mFigureVelSpinbox);
    mainLayout->addRow("Velocity Direction:", &mVelDirSpinbox);
    mainLayout->addRow("Initial Velocity:", &mInitVelSpinbox);
    mainLayout->addRow("Initial Velocity Random:", &mVelRandomSpinbox);
    mainLayout->addRow("Spread Vector:", &mSpreadVecSpinbox);


    connect(&mFigureVelSpinbox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.figureVel = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });

    connect(&mVelDirSpinbox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mProps.emitterVelDir = mVelDirSpinbox.getVector();
        emit propertiesUpdated(mProps);
    });

    connect(&mInitVelSpinbox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.initVel = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });

    connect(&mVelRandomSpinbox, &QDoubleSpinBox::valueChanged, this, [this](double value) {
        mProps.initVelRnd = static_cast<f32>(value);
        emit propertiesUpdated(mProps);
    });

    connect(&mSpreadVecSpinbox, &VectorSpinBoxBase::valueChanged, this, [this]() {
        mProps.spreadVec = mSpreadVecSpinbox.getVector();
        emit propertiesUpdated(mProps);
    });
}

void VelocityPropertiesWidget::setProperties(const Ptcl::VelocityProperties& properties) {
    QSignalBlocker b1(mFigureVelSpinbox);
    QSignalBlocker b2(mVelDirSpinbox);
    QSignalBlocker b3(mInitVelSpinbox);
    QSignalBlocker b4(mVelRandomSpinbox);
    QSignalBlocker b5(mSpreadVecSpinbox);

    mProps = properties;

    mFigureVelSpinbox.setValue(mProps.figureVel);
    mVelDirSpinbox.setVector(mProps.emitterVelDir);
    mInitVelSpinbox.setValue(mProps.initVel);
    mVelRandomSpinbox.setValue(mProps.initVelRnd);
    mSpreadVecSpinbox.setVector(mProps.spreadVec);

    update();
}

// ========================================================================== //
