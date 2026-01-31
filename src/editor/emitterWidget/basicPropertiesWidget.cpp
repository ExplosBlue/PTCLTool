#include "editor/emitterWidget/basicPropertiesWidget.h"
#include "util/nameValidator.h"

#include <QGridLayout>
#include <QLabel>

namespace PtclEditor {


// ========================================================================== //


EmitterWidget::BasicPropertiesWidget::BasicPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QGridLayout(this);

    auto addLabledWidget = [&](QWidget* widget, const QString& label, int row, int column, int span = 1) {
        mainLayout->addWidget(new QLabel(label), row, column);
        mainLayout->addWidget(widget, row, column + 1, 1, span);
    };

    // Emitter Name
    mNameLineEdit.setValidator(new EmitterNameValidator(&mNameLineEdit));
    addLabledWidget(&mNameLineEdit, "Emitter Name:", 0, 0, 3);
    connect(&mNameLineEdit, &QLineEdit::textChanged, this, [=, this](const QString& text) {
        mProps.name = text;
        emit propertiesUpdated(mProps);
    });

    // Emitter Type
    addLabledWidget(&mTypeComboBox, "Emitter Type:", 1, 0, 3);
    connect(&mTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.type = mTypeComboBox.currentEnum();
        emit propertiesUpdated(mProps);
        emit emitterTypeChanged();
    });

    // Random Seed Mode
    mRandomSeedMode.addItem("Random Per Emitter", static_cast<s32>(PtclSeed::Mode::RandomPerEmitter));
    mRandomSeedMode.addItem("Random Per EmitterSet", static_cast<s32>(PtclSeed::Mode::RandomPerSet));
    mRandomSeedMode.addItem("Constant Seed", static_cast<s32>(PtclSeed::Mode::ConstantSeed));
    addLabledWidget(&mRandomSeedMode, "Random Seed Mode:", 2, 0);
    connect(&mRandomSeedMode, &QComboBox::currentIndexChanged, this, [=, this]() {
        auto& seed = mProps.randomSeed;
        auto mode = static_cast<PtclSeed::Mode>(mRandomSeedMode.currentData().toUInt());

        seed.setMode(mode);
        mRandomSeedSpinBox.setEnabled(mode == PtclSeed::Mode::ConstantSeed);
        emit propertiesUpdated(mProps);
    });

    // Random Seed
    mRandomSeedSpinBox.setDisabled(false);
    mRandomSeedSpinBox.setRange(1, 0xFFFFFFFE);
    addLabledWidget(&mRandomSeedSpinBox, "Random Seed:", 2, 2);
    connect(&mRandomSeedSpinBox, &SizedSpinBoxBase::valueChanged, this, [=, this](int value) {
        auto& seed = mProps.randomSeed;
        if (seed.mode() == PtclSeed::Mode::ConstantSeed) {
            seed.setConstantSeed(static_cast<u32>(value));
        }
        emit propertiesUpdated(mProps);
    });

    // Follow Type
    mFollowTypeComboBox.setEnumText(Ptcl::FollowType::All, "Follow emitter exactly");
    mFollowTypeComboBox.setEnumText(Ptcl::FollowType::PosOnly, "Follow emitter position");
    mFollowTypeComboBox.setEnumText(Ptcl::FollowType::None, "Follow disabled");
    addLabledWidget(&mFollowTypeComboBox, "Follow Mode:", 3, 0, 3);
    connect(&mFollowTypeComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mFollowTypeComboBox.currentEnum();

        mProps.followType = type;
        // TODO: Check if this should also be set for Ptcl::FollowType::PosOnly
        mProps.isFollow = (type == Ptcl::FollowType::All);
        emit propertiesUpdated(mProps);
    });

    // Billboard Type
    addLabledWidget(&mBillboardComboBox, "Billboard Type:", 4, 0, 3);
    connect(&mBillboardComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        const auto type = mBillboardComboBox.currentEnum();

        mProps.billboardType = type;
        mProps.isPolygon = (type == Ptcl::BillboardType::PolygonXY || type == Ptcl::BillboardType::PolygonXZ);
        mProps.isVelLook = (type == Ptcl::BillboardType::VelLook || type == Ptcl::BillboardType::VelLookPolygon);
        mProps.isEmitterBillboardMtx = mProps.isVelLook;
        emit propertiesUpdated(mProps);
    });

}

void EmitterWidget::BasicPropertiesWidget::setProperties(const Ptcl::Emitter::BasicProperties& properties) {
    QSignalBlocker b1(mNameLineEdit);
    QSignalBlocker b2(mTypeComboBox);
    QSignalBlocker b3(mRandomSeedMode);
    QSignalBlocker b4(mRandomSeedSpinBox);
    QSignalBlocker b5(mFollowTypeComboBox);
    QSignalBlocker b6(mBillboardComboBox);

    mProps = properties;

    mNameLineEdit.setText(mProps.name);
    mTypeComboBox.setCurrentEnum(mProps.type);

    auto& randomSeed = mProps.randomSeed;
    auto seedMode = randomSeed.mode();
    int index = mRandomSeedMode.findData(static_cast<int>(seedMode));
    if (index != -1) {
        mRandomSeedMode.setCurrentIndex(index);
    }
    mRandomSeedSpinBox.setValue(randomSeed.constantSeed());
    mRandomSeedSpinBox.setEnabled(seedMode == PtclSeed::Mode::ConstantSeed);

    mFollowTypeComboBox.setCurrentEnum(mProps.followType);
    mBillboardComboBox.setCurrentEnum(mProps.billboardType);
}


// ========================================================================== //


} // namespace PtclEditor
