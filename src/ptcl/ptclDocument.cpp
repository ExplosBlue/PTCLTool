#include "ptcl/ptclCommand.h"
#include "ptcl/ptclDocument.h"

#include <utility>


namespace Ptcl {


// ========================================================================== //


Selection::Selection(QObject* parent) :
    QObject{parent} {}

void Selection::set(s32 setIndex, s32 emitterIndex, Type type) {
    mSetIndex = setIndex;
    mEmitterIndex = emitterIndex;
    mType = type;
    emit selectionChanged(mSetIndex, mEmitterIndex, mType);
}


// ========================================================================== //


Document::Document(QObject* parent) :
    QObject{parent} {}

bool Document::load(const QString& filePath) {
    mFilePath = filePath;
    mUndoStack.clear();
    mUndoStack.setClean();
    return mData.load(filePath);
}

bool Document::save(const QString& filePath) {
    mFilePath = filePath;
    mUndoStack.setClean();
    return mData.save(filePath);
}

void Document::setProjectName(const QString& name) {
    mUndoStack.push(new RenameProjectNameCommand(this, name));
}

void Document::addEmitter(QString label, s32 setIndex, std::unique_ptr<Emitter> emitter) {
    mUndoStack.push(new AddEmitterCommand(this, setIndex, std::move(label), std::move(emitter)));
}

void Document::removeEmitter(s32 setIndex, s32 emitterIndex) {
    mUndoStack.push(new RemoveEmitterCommand(this, setIndex, emitterIndex));
}

void Document::addEmitterSet(QString label, std::unique_ptr<EmitterSet> emitterSet) {
    mUndoStack.push(new AddEmitterSetCommand(this, std::move(label), std::move(emitterSet)));
}

void Document::removeEmitterSet(s32 setIndex) {
    mUndoStack.push(new RemoveEmitterSetCommand(this, setIndex));
}


// ========================================================================== //


} // namespace Ptcl
