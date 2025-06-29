#include "editor/rotationPropertiesWidget.h"
#include "util/mathUtil.h"

#include <QFormLayout>

// ========================================================================== //


RotationPropertiesWidget::RotationPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout;

    // RotType
    mainLayout->addRow("Rotation Type", &mRotTypeSpinBox);
    connect(&mRotTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        if (!mEmitterPtr || misPopulating) {
            return;
        }

        mEmitterPtr->setRotType(mRotTypeSpinBox.currentEnum());
        updateAxis();
    });

    auto deg2idxVec = [](const QVector3D& v) {
        Ptcl::binVec3i result;

        result.x = MathUtil::deg2idx(v.x());
        result.y = MathUtil::deg2idx(v.y());
        result.z = MathUtil::deg2idx(v.z());

        return result;
    };

    // Initial Rotation
    mainLayout->addRow("Initial Rotation", &mInitRotSpinBox);
    connect(&mInitRotSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        if (!mEmitterPtr || misPopulating) {
            return;
        }

        mEmitterPtr->setInitRot(deg2idxVec(mInitRotSpinBox.getVector()));
    });

    // Initial Rotation Rand
    mainLayout->addRow("Initial Rotation Random", &mInitRotRandSpinBox);
    connect(&mInitRotRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        if (!mEmitterPtr || misPopulating) {
            return;
        }

        mEmitterPtr->setInitRotRand(deg2idxVec(mInitRotRandSpinBox.getVector()));
    });

    // Rotation Speed
    mainLayout->addRow("Rotation Speed", &mRotVelSpinBox);
    connect(&mRotVelSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        if (!mEmitterPtr || misPopulating) {
            return;
        }

        mEmitterPtr->setRotVel(deg2idxVec(mRotVelSpinBox.getVector()));
    });

    // Rotation Speed Rand
    mainLayout->addRow("Rotation Speed Random", &mRotVelRandSpinBox);
    connect(&mRotVelRandSpinBox, &VectorSpinBoxBase::valueChanged, this, [this, deg2idxVec]() {
        if (!mEmitterPtr || misPopulating) {
            return;
        }

        mEmitterPtr->setRotVelRand(deg2idxVec(mRotVelRandSpinBox.getVector()));
    });

    setLayout(mainLayout);

}

void RotationPropertiesWidget::setEmitter(Ptcl::Emitter* emitter) {
    mEmitterPtr = emitter;
    populateWidgets();
}

void RotationPropertiesWidget::populateWidgets() {
    if (!mEmitterPtr) {
        return;
    }

    misPopulating = true;

    auto idx2degVec = [](const Ptcl::binVec3i& v) {
        return QVector3D {
            MathUtil::to180(MathUtil::idx2deg(v.x)),
            MathUtil::to180(MathUtil::idx2deg(v.y)),
            MathUtil::to180(MathUtil::idx2deg(v.z))
        };
    };

    mRotTypeSpinBox.setCurrentEnum(mEmitterPtr->rotType());

    mInitRotSpinBox.setVector(idx2degVec(mEmitterPtr->initRot()));
    mInitRotRandSpinBox.setVector(idx2degVec(mEmitterPtr->initRotRand()));
    mRotVelSpinBox.setVector(idx2degVec(mEmitterPtr->rotVel()));
    mRotVelRandSpinBox.setVector(idx2degVec(mEmitterPtr->rotVelRand()));

    updateAxis();

    misPopulating = false;
}

void RotationPropertiesWidget::updateAxis() {
    if (!mEmitterPtr) {
        return;
    }

    auto rotType = mEmitterPtr->rotType();

    using Axis = VectorSpinBoxBase::Axis;

    switch (rotType) {
    case Ptcl::RotType::None:
        mInitRotSpinBox.setEnabledAxis(Axis::None);
        mInitRotRandSpinBox.setEnabledAxis(Axis::None);
        mRotVelSpinBox.setEnabledAxis(Axis::None);
        mRotVelRandSpinBox.setEnabledAxis(Axis::None);
        break;
    case Ptcl::RotType::RotX:
        mInitRotSpinBox.setEnabledAxis(Axis::X);
        mInitRotRandSpinBox.setEnabledAxis(Axis::X);
        mRotVelSpinBox.setEnabledAxis(Axis::X);
        mRotVelRandSpinBox.setEnabledAxis(Axis::X);
        break;
    case Ptcl::RotType::RotY:
        mInitRotSpinBox.setEnabledAxis(Axis::Y);
        mInitRotRandSpinBox.setEnabledAxis(Axis::Y);
        mRotVelSpinBox.setEnabledAxis(Axis::Y);
        mRotVelRandSpinBox.setEnabledAxis(Axis::Y);
        break;
    case Ptcl::RotType::RotZ:
        mInitRotSpinBox.setEnabledAxis(Axis::Z);
        mInitRotRandSpinBox.setEnabledAxis(Axis::Z);
        mRotVelSpinBox.setEnabledAxis(Axis::Z);
        mRotVelRandSpinBox.setEnabledAxis(Axis::Z);
        break;
    case Ptcl::RotType::RotXYZ:
        mInitRotSpinBox.setEnabledAxis(Axis::XYZ);
        mInitRotRandSpinBox.setEnabledAxis(Axis::XYZ);
        mRotVelSpinBox.setEnabledAxis(Axis::XYZ);
        mRotVelRandSpinBox.setEnabledAxis(Axis::XYZ);
        break;
    default:
        break;
    }

}


// ========================================================================== //
