#include "editor/inspector/inspectorWidgetBase.h"

#include <QFormLayout>
#include <QFrame>
#include <QLabel>


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
        connect(selection, &Ptcl::Selection::selectionChanged, this, [this](s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type /*type*/) {
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

void InspectorWidgetBase::addSectionHeader(QVBoxLayout* layout, const QString& title, QWidget* parent) {
    auto* row = new QHBoxLayout;
    row->setContentsMargins(0, 4, 0, 0);
    row->setSpacing(8);

    auto* label = new QLabel(title, parent);
    label->setStyleSheet("font-weight: bold;");
    row->addWidget(label);

    auto* line = new QFrame(parent);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    row->addWidget(line);

    layout->addLayout(row);
}

void InspectorWidgetBase::addSectionHeader(QFormLayout* layout, const QString& title, QWidget* parent) {
    auto* row = new QHBoxLayout;
    row->setContentsMargins(0, 4, 0, 0);
    row->setSpacing(8);

    auto* label = new QLabel(title, parent);
    label->setStyleSheet("font-weight: bold;");
    row->addWidget(label);

    auto* line = new QFrame(parent);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    row->addWidget(line);

    auto* container = new QWidget(parent);
    container->setLayout(row);
    layout->addRow(container);
}

// ========================================================================== //


} // namespace PtclEditor
