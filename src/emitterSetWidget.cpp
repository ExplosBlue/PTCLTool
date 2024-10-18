#include "emitterSetWidget.h"

EmitterSetWidget::EmitterSetWidget(QWidget* parent) :
    QWidget(parent) {

    // Setup widgets
    mNameLineEdit.setPlaceholderText("EmitterSetName");
    mEmitterCountLabel.setText("Emitter Count:");

    // Add widgets to layout

    // Qt is cringe...
    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("EmitterSet Name:"));
    nameLayout->addWidget(&mNameLineEdit);
    mMainLayout.addLayout(nameLayout);

    mMainLayout.addWidget(&mEmitterCountLabel);
    mMainLayout.addWidget(&mEmitterPicker);
    mMainLayout.addWidget(&mEmitterWidget);

    mMainLayout.addSpacerItem(new QSpacerItem(0, 1000, QSizePolicy::Expanding, QSizePolicy::Expanding));

    setLayout(&mMainLayout);

    // Setup signals
    connect(&mEmitterPicker, &QComboBox::currentIndexChanged, this, &EmitterSetWidget::selectedEmitterChanged);
    // TODO: Connect other signals...
}

void EmitterSetWidget::setEmitterSet(Ptcl::EmitterSet* emitterSet) {

    mEmitterSetPtr = emitterSet;

    // Update Emitter Picker
    populateEmitterPicker();

    // Update Properties
    populateProperties();
}

void EmitterSetWidget::selectedEmitterChanged(u32 index) {

    if (index < mEmitterSetPtr->emitters().size()) {
        mEmitterWidget.setEmitter(&mEmitterSetPtr->emitters()[index]);
    }
}

void EmitterSetWidget::populateEmitterPicker() {

    mEmitterPicker.blockSignals(true);
    mEmitterPicker.clear();

    u32 idx = 0;
    for (auto& emitter : mEmitterSetPtr->emitters()) {
        mEmitterPicker.addItem(emitter.name(), idx);
        ++idx;
    }

    mEmitterPicker.blockSignals(false);
    mEmitterWidget.setEmitter(&mEmitterSetPtr->emitters()[0]);
}

void EmitterSetWidget::populateProperties() {

    mNameLineEdit.setText(mEmitterSetPtr->name());
    mEmitterCountLabel.setText("Emitter Count: " + QString::number(mEmitterSetPtr->emitterCount()));
}
