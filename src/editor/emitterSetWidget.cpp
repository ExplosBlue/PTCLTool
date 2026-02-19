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
    mUserDataSpinBox.setRange(std::numeric_limits<u32>::min(), std::numeric_limits<u32>::max());
    mLastUpdateSpinBox.setRange(std::numeric_limits<u32>::min(), std::numeric_limits<u32>::max());

    auto* mainLayout = new QFormLayout(this);
    mainLayout->addRow("EmitterSet Name:", &mNameLineEdit);
    mainLayout->addRow("UserData:", &mUserDataSpinBox);
    mainLayout->addRow("LastUpdate:", &mLastUpdateSpinBox);

    setupConnections();
}

void EmitterSetWidget::setupConnections() {
    // Name Edit
    connect(&mNameLineEdit, &QLineEdit::textEdited, this, [this](const QString& text) {
        if (!mEmitterSet) {
            return;
        }
        mEmitterSet->setName(text);
        emit emitterSetNamedChanged();
        emit propertiesChanged();
    });

    // User Data
    connect(&mUserDataSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mEmitterSet->setUserData(static_cast<u32>(value));
        emit propertiesChanged();
    });

    // Last Update
    connect(&mLastUpdateSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mEmitterSet->setLastUpdateDate(static_cast<u32>(value));
        emit propertiesChanged();
    });
}

void EmitterSetWidget::setDocument(Ptcl::Document* document) {
    mDocument = document;
}

void EmitterSetWidget::setSelection(Ptcl::Selection* selection) {
    mSelection = selection;

    connect(selection, &Ptcl::Selection::selectionChanged, this, [this](s32 setIndex, s32 emitterIndex) {
        Q_UNUSED(emitterIndex);

        if (!mDocument) {
            mEmitterSet = nullptr;
            setEnabled(false);
            return;
        }

        mEmitterSet = mDocument->emitterSet(setIndex);
        setEnabled(true);
        populateProperties();
    });
}

void EmitterSetWidget::populateProperties() {
    blockSignals(true);

    mNameLineEdit.setText(mEmitterSet->name());
    mUserDataSpinBox.setValue(mEmitterSet->userData());
    mLastUpdateSpinBox.setValue(mEmitterSet->lastUpdateDate());

    blockSignals(false);
}


// ========================================================================== //


} // namespace PtclEditor
