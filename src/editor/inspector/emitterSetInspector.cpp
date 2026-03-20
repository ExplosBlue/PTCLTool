#include "editor/inspector/emitterSetInspector.h"
#include "util/nameValidator.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


EmitterSetInspector::EmitterSetInspector(QWidget* parent) :
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

void EmitterSetInspector::setupConnections() {
    // Name Edit
    connect(&mNameLineEdit, &QLineEdit::textEdited, this, [this](const QString& text) {
        mDocument->setEmitterSetProperty(
            mSelection->emitterSetIndex(),
            "Set EmitterSet Name",
            "SetEmitterSetName",
            &Ptcl::EmitterSet::name,
            &Ptcl::EmitterSet::setName,
            text
        );
    });

    // User Data
    connect(&mUserDataSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mDocument->setEmitterSetProperty(
            mSelection->emitterSetIndex(),
            "Set EmitterSet UserData",
            "SetEmitterSetUserData",
            &Ptcl::EmitterSet::userData,
            &Ptcl::EmitterSet::setUserData,
            static_cast<u32>(value)
        );
    });

    // Last Update
    connect(&mLastUpdateSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mDocument->setEmitterSetProperty(
            mSelection->emitterSetIndex(),
            "Set EmitterSet LastUpdate",
            "SetEmitterSetLastUpdate",
            &Ptcl::EmitterSet::lastUpdateDate,
            &Ptcl::EmitterSet::setLastUpdateDate,
            static_cast<u32>(value)
        );
    });
}

void EmitterSetInspector::setDocument(Ptcl::Document* document) {
    if (mDocument) {
        mDocument->disconnect(this);
    }

    mDocument = document;

    if (mDocument) {
        connect(mDocument, &Ptcl::Document::emitterSetChanged, this, [this](s32 setIndex) {
            if (!mEmitterSet) {
                return;
            }

            if (setIndex != mSelection->emitterSetIndex()) {
                return;
            }

            populateProperties();
        });
    }
}

void EmitterSetInspector::setSelection(Ptcl::Selection* selection) {
    if (mSelection) {
        mSelection->disconnect(this);
    }

    mSelection = selection;

    if (mSelection) {
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
}

void EmitterSetInspector::populateProperties() {
    QSignalBlocker b1(mNameLineEdit);
    QSignalBlocker b2(mUserDataSpinBox);
    QSignalBlocker b3(mLastUpdateSpinBox);

    mNameLineEdit.setText(mEmitterSet->name());
    mUserDataSpinBox.setValue(mEmitterSet->userData());
    mLastUpdateSpinBox.setValue(mEmitterSet->lastUpdateDate());
}


// ========================================================================== //


} // namespace PtclEditor
