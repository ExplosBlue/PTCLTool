#include "editor/inspector/inspectorWidgetBase.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


InspectorWidgetBase::InspectorWidgetBase(QWidget* parent) :
    QWidget{parent} {}

void InspectorWidgetBase::setDocument(Ptcl::Document* document) {
    if (mDocument) {
        mDocument->disconnect(this);
    }

    mDocument = document;

    if (mDocument) {
        connect(mDocument, &Ptcl::Document::emitterChanged, this, &InspectorWidgetBase::onEmitterChanged);
    }
}

void InspectorWidgetBase::setSelection(Ptcl::Selection* selection) {
    if (mSelection) {
        mSelection->disconnect(this);
    }

    mSelection = selection;

    if (mSelection) {
        connect(selection, &Ptcl::Selection::selectionChanged, this, [this](s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type type) {
            if (!mDocument) {
                mEmitter = nullptr;
                setEnabled(false);
                return;
            }

            mEmitter = mDocument->emitter(setIndex, emitterIndex);

            setEnabled(true);
            populateProperties();
        });
    }
}

QString InspectorWidgetBase::formatHistoryLabel(const QString& label) const {
    return QString("Set %1, Emitter %2 - %3")
    .arg(mSelection->emitterSetIndex())
        .arg(mSelection->emitterIndex())
        .arg(label);
}


void InspectorWidgetBase::onEmitterChanged(s32 setIndex, s32 emitterIndex) {
    if (!mEmitter) {
        return;
    }

    if (setIndex != mSelection->emitterSetIndex() || emitterIndex != mSelection->emitterIndex()) {
        return;
    }

    populateProperties();
}

// ========================================================================== //


} // namespace PtclEditor
