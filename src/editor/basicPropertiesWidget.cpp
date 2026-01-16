#include "editor/basicPropertiesWidget.h"
#include "util/nameValidator.h"

#include <QGridLayout>
#include <QLabel>

// ========================================================================== //


BasicPropertiesWidget::BasicPropertiesWidget(QWidget* parent) :
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
    mFollowTypeSpinBox.setEnumText(Ptcl::FollowType::All, "Follow emitter exactly");
    mFollowTypeSpinBox.setEnumText(Ptcl::FollowType::PosOnly, "Follow emitter position");
    mFollowTypeSpinBox.setEnumText(Ptcl::FollowType::None, "Follow disabled");
    addLabledWidget(&mFollowTypeSpinBox, "Follow Mode:", 3, 0, 3);
    connect(&mFollowTypeSpinBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.followType = mFollowTypeSpinBox.currentEnum();
        emit propertiesUpdated(mProps);
    });
}

void BasicPropertiesWidget::setProperties(const Ptcl::BasicProperties& properties) {
    QSignalBlocker b1(mNameLineEdit);
    QSignalBlocker b2(mTypeComboBox);
    QSignalBlocker b3(mRandomSeedMode);
    QSignalBlocker b4(mRandomSeedSpinBox);
    QSignalBlocker b5(mFollowTypeSpinBox);

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

    mFollowTypeSpinBox.setCurrentEnum(mProps.followType);
}


// ========================================================================== //
