#include "editor/inspector/emitterSetInspector.h"
#include "util/nameValidator.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


EmitterSetInspector::EmitterSetInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    auto* mainLayout = new QFormLayout(this);
    mainLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    addSectionHeader(mainLayout, "Emitter Set", this);

    mNameLineEdit.setPlaceholderText("EmitterSetName");
    mNameLineEdit.setValidator(new EmitterNameValidator(&mNameLineEdit));
    mNameLineEdit.setToolTip("The name of this emitter set.");
    mainLayout->addRow("Name:", &mNameLineEdit);

    setupConnections();
}


void EmitterSetInspector::setupConnections() {
    connect(&mNameLineEdit, &QLineEdit::textEdited, this, [this](const QString& text) {
        if (!mDocument || !mSelection) { return; }
        mDocument->setEmitterSetProperty(
            mSelection->emitterSetIndex(),
            QStringLiteral("Set EmitterSet Name"),
            QStringLiteral("SetEmitterSetName"),
            &Ptcl::EmitterSet::name,
            &Ptcl::EmitterSet::setName,
            text
        );
    });
}


void EmitterSetInspector::setDocument(Ptcl::Document* document) {
    if (mDocument) {
        mDocument->disconnect(this);
    }

    mDocument = document;

    if (!mDocument) { return; }

    connect(mDocument, &Ptcl::Document::emitterSetChanged, this, [this](s32 setIndex) {
        if (!mEmitterSet || !mSelection) { return; }
        if (setIndex != mSelection->emitterSetIndex()) { return; }
        populateProperties();
    });
}


void EmitterSetInspector::setSelection(Ptcl::Selection* selection) {
    if (mSelection) {
        mSelection->disconnect(this);
    }

    mSelection = selection;

    if (mSelection) {
        connect(mSelection, &Ptcl::Selection::selectionChanged, this, [this](s32 setIndex, s32 emitterIndex) {
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

    if (!mEmitterSet) {
        return;
    }

    mNameLineEdit.setText(mEmitterSet->name());
}


// ========================================================================== //


} // namespace PtclEditor
