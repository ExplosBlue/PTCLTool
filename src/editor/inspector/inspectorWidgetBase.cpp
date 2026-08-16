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
    mEmitter = nullptr;

    if (mDocument) {
        connect(mDocument, &Ptcl::Document::emitterChanged, this, &InspectorWidgetBase::onEmitterChanged);
        connect(mDocument, &Ptcl::Document::textureChanged, this, &InspectorWidgetBase::onTextureChanged);
    }
}

void InspectorWidgetBase::setSelection(Ptcl::Selection* selection) {
    if (mSelection) {
        mSelection->disconnect(this);
    }

    mSelection = selection;

    if (mSelection) {
        connect(selection, &Ptcl::Selection::selectionChanged, this, [this](s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type type) {
            Q_UNUSED(type);

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

void InspectorWidgetBase::onTextureChanged(s32 index) {
    Q_UNUSED(index);
}

void InspectorWidgetBase::addSectionHeader(QVBoxLayout* layout, const QString& title, QWidget* parent) {
    auto* row = createHeaderRow(title, parent);
    layout->addLayout(row);
}

void InspectorWidgetBase::addSectionHeader(QFormLayout* layout, const QString& title, QWidget* parent) {
    auto* row = createHeaderRow(title, parent);
    auto* container = new QWidget(parent);
    container->setLayout(row);
    layout->addRow(container);
}

QHBoxLayout* InspectorWidgetBase::createHeaderRow(const QString& title, QWidget* parent) {
    auto* row = new QHBoxLayout;
    row->setContentsMargins(0, 4, 0, 0);
    row->setSpacing(8);

    auto* label = new QLabel(title, parent);
    QFont font = label->font();
    font.setBold(true);
    label->setFont(font);
    row->addWidget(label);

    auto* line = new QFrame(parent);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    row->addWidget(line);

    return row;
}


// ========================================================================== //


} // namespace PtclEditor
