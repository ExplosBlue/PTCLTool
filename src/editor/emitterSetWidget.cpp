#include "editor/emitterSetWidget.h"
#include "util/nameValidator.h"

#include <QMessageBox>


namespace PtclEditor {


// ========================================================================== //


EmitterSetWidget::EmitterSetWidget(QWidget* parent) :
    QWidget{parent} {
    // Emitter Widget
    connect(&mEmitterWidget, &EmitterWidget::textureUpdated, this, [this](int oldIndex, int index) {
        emit textureUpdated(oldIndex, index);
    });

    connect(&mEmitterWidget, &EmitterWidget::emitterNameChanged, this, [this]() {
        emit emitterNameChanged();
    });

    connect(&mEmitterWidget, &EmitterWidget::emitterTypeChanged, this, [this]() {
        emit emitterTypeChanged();
    });

    connect(&mEmitterWidget, &EmitterWidget::complexFlagsChanged, this, [this]() {
        emit emitterComplexFlagsChanged();
    });

    // Name Edit
    mNameLineEdit.setPlaceholderText("EmitterSetName");
    mNameLineEdit.setValidator(new EmitterNameValidator(&mNameLineEdit));
    connect(&mNameLineEdit, &QLineEdit::textEdited, this, [this](const QString& text) {
        if (!mEmitterSetPtr) {
            return;
        }
        mEmitterSetPtr->setName(text);
        emit emitterSetNamedChanged();
    });

    // Name layout
    auto* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("EmitterSet Name:"));
    nameLayout->addWidget(&mNameLineEdit);

    // Count Label
    mEmitterCountLabel.setText("Emitter Count:");

    // Main Layout
    mMainLayout.addLayout(nameLayout);
    mMainLayout.addWidget(&mEmitterCountLabel);
    mMainLayout.addWidget(&mEmitterWidget);

    setLayout(&mMainLayout);
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

    mEmitterWidget.setEmitter(mEmitterSetPtr->emitters()[emitterIndex].get());
}

void EmitterSetWidget::setTextureList(const Ptcl::TextureList& textureList) {
    mEmitterWidget.setTextureList(&textureList);
};

void EmitterSetWidget::showStandardEditor() {
    mEmitterWidget.showStandardEditor();
}

void EmitterSetWidget::showChildEditor() {
    mEmitterWidget.showChildEditor();
}

void EmitterSetWidget::showFluctuationEditor() {
    mEmitterWidget.showFluctuationEditor();
}

void EmitterSetWidget::showFieldEditor() {
    mEmitterWidget.showFieldEditor();
}

void EmitterSetWidget::clear() {
    setEnabled(false);
    mEmitterWidget.clear();
    mEmitterSetPtr = nullptr;
}

void EmitterSetWidget::populateProperties() {
    mNameLineEdit.setText(mEmitterSetPtr->name());
    mEmitterCountLabel.setText("Emitter Count: " + QString::number(mEmitterSetPtr->emitterCount()));
}


// ========================================================================== //


} // namespace PtclEditor
