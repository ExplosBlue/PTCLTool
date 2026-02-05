#include "editor/emitterSetWidget.h"
#include "util/nameValidator.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


EmitterSetWidget::EmitterSetWidget(QWidget* parent) :
    QWidget{parent} {
    mNameLineEdit.setPlaceholderText("EmitterSetName");
    mNameLineEdit.setValidator(new EmitterNameValidator(&mNameLineEdit));
    // TODO: Check these
    mUserDataSpinBox.setRange(std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());
    mLastUpdateSpinBox.setRange(std::numeric_limits<s32>::min(), std::numeric_limits<s32>::max());

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("EmitterSet Name:", &mNameLineEdit);
    mainLayout->addRow("UserData:", &mUserDataSpinBox);
    mainLayout->addRow("LastUpdate:", &mLastUpdateSpinBox);

    setupConnections();
}

void EmitterSetWidget::setupConnections() {
    // Name Edit
    connect(&mNameLineEdit, &QLineEdit::textEdited, this, [this](const QString& text) {
        if (!mEmitterSetPtr) {
            return;
        }
        mEmitterSetPtr->setName(text);
        emit emitterSetNamedChanged();
        emit propertiesChanged();
    });

    // User Data
    connect(&mUserDataSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mEmitterSetPtr->setUserData(value);
        emit propertiesChanged();
    });

    // Last Update
    connect(&mLastUpdateSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mEmitterSetPtr->setLastUpdateDate(value);
        emit propertiesChanged();
    });
}

void EmitterSetWidget::setEmitterSet(Ptcl::EmitterSet* emitterSet) {
    setEnabled(true);
    mEmitterSetPtr = emitterSet;
    populateProperties();
}

void EmitterSetWidget::clear() {
    setEnabled(false);
    mEmitterSetPtr = nullptr;
}

void EmitterSetWidget::populateProperties() {
    mNameLineEdit.setText(mEmitterSetPtr->name());
    mUserDataSpinBox.setValue(mEmitterSetPtr->userData());
    mLastUpdateSpinBox.setValue(mEmitterSetPtr->lastUpdateDate());
}


// ========================================================================== //


} // namespace PtclEditor
