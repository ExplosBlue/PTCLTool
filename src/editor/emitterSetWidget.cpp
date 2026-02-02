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

    auto* settingsLayout = new QFormLayout;
    settingsLayout->addRow("EmitterSet Name:", &mNameLineEdit);
    settingsLayout->addRow("UserData:", &mUserDataSpinBox);
    settingsLayout->addRow("LastUpdate:", &mLastUpdateSpinBox);

    mEmitterGroup.setFlat(true);
    auto* emitterGroupLayout = new QVBoxLayout(&mEmitterGroup);
    emitterGroupLayout->setContentsMargins(0, 0, 0, 0);
    emitterGroupLayout->addWidget(&mEmitterWidget);

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(settingsLayout);
    mainLayout->addWidget(&mEmitterGroup);

    setupConnections();
}

void EmitterSetWidget::setupConnections() {
    // Emitter Widget
    connect(&mEmitterWidget, &EmitterWidget::textureUpdated, this, [this](int oldIndex, int index) {
        emit textureUpdated(oldIndex, index);
    });

    connect(&mEmitterWidget, &EmitterWidget::emitterNameChanged, this, [this]() {
        updateStatusLabel();
        emit emitterNameChanged();
    });

    connect(&mEmitterWidget, &EmitterWidget::emitterTypeChanged, this, [this]() {
        emit emitterTypeChanged();
    });

    connect(&mEmitterWidget, &EmitterWidget::complexFlagsChanged, this, [this]() {
        emit emitterComplexFlagsChanged();
    });

    // Name Edit
    connect(&mNameLineEdit, &QLineEdit::textEdited, this, [this](const QString& text) {
        if (!mEmitterSetPtr) {
            return;
        }
        mEmitterSetPtr->setName(text);
        updateStatusLabel();
        emit emitterSetNamedChanged();
    });

    // User Data
    connect(&mUserDataSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mEmitterSetPtr->setUserData(value);
    });

    // Last Update
    connect(&mLastUpdateSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        mEmitterSetPtr->setLastUpdateDate(value);
    });
}

void EmitterSetWidget::setEmitterSet(Ptcl::EmitterSet* emitterSet) {
    setEnabled(true);
    mEmitterSetPtr = emitterSet;
    populateProperties();
}

void EmitterSetWidget::selectEmitter(s32 emitterIndex) {
    if (emitterIndex < 0 || emitterIndex >= mEmitterSetPtr->emitterCount()) {
        return;
    }

    const auto& emitter = mEmitterSetPtr->emitters()[emitterIndex];
    mEmitterWidget.setEmitter(emitter.get());
    mCurEmitterIdx = emitterIndex;
    updateStatusLabel();
}

void EmitterSetWidget::setTextureList(const Ptcl::TextureList& textureList) {
    mEmitterWidget.setTextureList(&textureList);
};

void EmitterSetWidget::showStandardEditor() {
    mEmitterWidget.showStandardEditor();
    mEditorMode = EditorMode::Standard;
    updateStatusLabel();
}

void EmitterSetWidget::showChildEditor() {
    mEmitterWidget.showChildEditor();
    mEditorMode = EditorMode::Child;
    updateStatusLabel();
}

void EmitterSetWidget::showFluctuationEditor() {
    mEmitterWidget.showFluctuationEditor();
    mEditorMode = EditorMode::Fluctuation;
    updateStatusLabel();
}

void EmitterSetWidget::showFieldEditor() {
    mEmitterWidget.showFieldEditor();
    mEditorMode = EditorMode::Field;
    updateStatusLabel();
}

void EmitterSetWidget::clear() {
    setEnabled(false);
    mEmitterWidget.clear();
    mEmitterSetPtr = nullptr;
}

void EmitterSetWidget::populateProperties() {
    mNameLineEdit.setText(mEmitterSetPtr->name());
    mUserDataSpinBox.setValue(mEmitterSetPtr->userData());
    mLastUpdateSpinBox.setValue(mEmitterSetPtr->lastUpdateDate());
}

void EmitterSetWidget::updateStatusLabel() {
    if (!mEmitterSetPtr || mCurEmitterIdx < 0) {
        mEmitterGroup.setTitle({});
        return;
    }

    const auto& emitter = mEmitterSetPtr->emitters()[mCurEmitterIdx];

    auto text = QString("%1 > %2").arg(mEmitterSetPtr->name(), emitter->name());

    switch (mEditorMode) {
    case EditorMode::Child:
        text += " > ChildData";
        break;
    case EditorMode::Fluctuation:
        text += " > Fluctuation";
        break;
    case EditorMode::Field:
        text += " > Field";
        break;
    case EditorMode::Standard:
        break;
    }
    mEmitterGroup.setTitle(text);
}


// ========================================================================== //


} // namespace PtclEditor
