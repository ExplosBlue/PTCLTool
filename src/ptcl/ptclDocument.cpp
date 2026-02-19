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


// ========================================================================== //


} // namespace Ptcl
