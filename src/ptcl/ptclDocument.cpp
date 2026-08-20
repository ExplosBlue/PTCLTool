#include "ptcl/ptclCommand.h"
#include "ptcl/ptclDocument.h"
#include "ptcl/ptcljson.h"

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
    mUndoStack.clear();
    mUndoStack.setClean();

    if (!mData.load(filePath)) {
        return false;
    }

    if (!filePath.endsWith(".ptclproj")) {
        mFilePath = filePath;
    }

    return true;
}

bool Document::save(const QString& filePath) {
    if (!mData.save(filePath)) {
        return false;
    }

    mFilePath = filePath;
    mUndoStack.setClean();
    return true;
}

bool Document::exportProject(const QString& dirPath) {
    if (!mData.exportProject(dirPath)) {
        return false;
    }

    return true;
}

bool Document::exportEmitter(s32 setIndex, s32 emitterIndex, const QString& filePath) {
    const auto* emitter = mData.emitter(setIndex, emitterIndex);
    if (!emitter) {
        return false;
    }

    return PtclJson::exportEmitter(*emitter, filePath);
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

void Document::addTexture(std::unique_ptr<Texture> texture) {
    mUndoStack.push(new AddTextureCommand(this, std::move(texture)));
}

void Document::removeTexture(s32 index) {
    mUndoStack.push(new RemoveTextureCommand(this, index));
}

void Document::replaceTexture(s32 index, std::unique_ptr<Texture> texture, const QString& label) {
    mUndoStack.push(new ReplaceTextureCommand(this, index, std::move(texture), label));
}


// ========================================================================== //


} // namespace Ptcl
