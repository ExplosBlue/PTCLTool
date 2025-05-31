#include "editor/emitterSetWidget.h"

namespace PtclEditor {

EmitterSetWidget::EmitterSetWidget(QWidget* parent) :
    QWidget(parent) {

    // Emitter Widget
    connect(&mEmitterWidget, &EmitterWidget::textureUpdated, this, [=, this](int oldIndex, int index) {
        emit textureUpdated(oldIndex, index);
    });

    // Setup widgets
    mNameLineEdit.setPlaceholderText("EmitterSetName");
    mEmitterCountLabel.setText("Emitter Count:");

    // Add widgets to layout
    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(new QLabel("EmitterSet Name:"));
    nameLayout->addWidget(&mNameLineEdit);
    mMainLayout.addLayout(nameLayout);

    mMainLayout.addWidget(&mEmitterCountLabel);
    mMainLayout.addWidget(&mEmitterTabs, 1);

    setLayout(&mMainLayout);

    // Setup signals
    connect(&mEmitterTabs, &QTabWidget::currentChanged, this, &EmitterSetWidget::selectedEmitterChanged);
}

void EmitterSetWidget::setEmitterSet(Ptcl::EmitterSet* emitterSet) {
    mEmitterSetPtr = emitterSet;
    populateEmitterPicker();
    populateProperties();
}

EmitterWidget& EmitterSetWidget::getEmitterWidget() {
    return mEmitterWidget;
}

void EmitterSetWidget::selectedEmitterChanged(u32 index) {
    if (!mEmitterSetPtr) {
        return;
    }

    const int lastIndex = mEmitterTabs.count() - 1;

    // Create new emitter if "+" tab selected
    if (index == lastIndex) {
        auto newEmitter = std::make_unique<Ptcl::Emitter>();
        newEmitter->setName("New_Emitter_" + QString::number(mEmitterSetPtr->emitterCount() + 1));

        mEmitterSetPtr->emitters().push_back(std::move(newEmitter));

        mEmitterCountLabel.setText("Emitter Count: " + QString::number(mEmitterSetPtr->emitterCount()));
        populateEmitterPicker();

        mEmitterTabs.setCurrentIndex(lastIndex - 1);

        return;
    }

    if (index >= mEmitterTabPlaceholders.size() || index >= mEmitterSetPtr->emitters().size()) {
        return;
    }

    // Remove from old parent
    mEmitterWidget.setParent(nullptr);

    // Reparent to new placeholder and update layout
    QWidget* tabPlaceholder = mEmitterTabPlaceholders[index];
    mEmitterWidget.setParent(tabPlaceholder);
    if (tabPlaceholder->layout()) {
        tabPlaceholder->layout()->addWidget(&mEmitterWidget);
    }

    // Update emitter content
    mEmitterWidget.setEmitter(mEmitterSetPtr->emitters()[index].get());
}

void EmitterSetWidget::populateEmitterPicker() {
    mEmitterTabs.blockSignals(true);
    mEmitterTabs.clear();
    mEmitterTabPlaceholders.clear();

    for (auto& emitter : mEmitterSetPtr->emitters()) {
        QWidget* placeholder = new QWidget();
        QVBoxLayout* layout = new QVBoxLayout(placeholder);
        layout->setContentsMargins(0, 0, 0, 0);
        placeholder->setLayout(layout);

        mEmitterTabs.addTab(placeholder, emitter->name());
        mEmitterTabPlaceholders.push_back(placeholder);
    }

    // Add a tab to add new emitters
    QWidget* addTabPlaceholder = new QWidget();
    mEmitterTabs.addTab(addTabPlaceholder, "+");

    // Add emitter widget to the initial tab's placeholder
    int currentIndex = mEmitterTabs.currentIndex();
    if (currentIndex >= 0 && currentIndex < mEmitterTabPlaceholders.size()) {
        mEmitterWidget.setParent(mEmitterTabPlaceholders[currentIndex]);
        mEmitterTabPlaceholders[currentIndex]->layout()->addWidget(&mEmitterWidget);
        mEmitterWidget.setEmitter(mEmitterSetPtr->emitters()[currentIndex].get());
    }

    mEmitterTabs.blockSignals(false);
}

void EmitterSetWidget::populateProperties() {
    mNameLineEdit.setText(mEmitterSetPtr->name());
    mEmitterCountLabel.setText("Emitter Count: " + QString::number(mEmitterSetPtr->emitterCount()));
}

} // namespace PtclEditor
