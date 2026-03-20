#include "ptcl/ptclCommand.h"
#include "ptcl/ptclDocument.h"


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
    mModified = false;
    return mData.load(filePath);
}

bool Document::save(const QString& filePath) {
    mFilePath = filePath;
    mModified = false;
    return mData.save(filePath);
}

void Document::setProjectName(const QString& name) {
    mUndoStack.push(new RenameProjectNameCommand(this, name));
}

void Document::addEmitter(s32 setIndex, std::unique_ptr<Emitter> emitter) {
    mUndoStack.push(new AddEmitterCommand(this, setIndex, std::move(emitter)));
}

void Document::removeEmitter(s32 setIndex, s32 emitterIndex) {
    mUndoStack.push(new RemoveEmitterCommand(this, setIndex, emitterIndex));
}


// ========================================================================== //


} // namespace Ptcl
