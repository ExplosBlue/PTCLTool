#pragma once

#include "ptcl/ptclEmitter.h"
#include "ptcl/ptclDocument.h"


#include <QUndoCommand>
#include <utility>


namespace Ptcl {


// ========================================================================== //


class DocumentCommandBase : public QUndoCommand {
public:
    explicit DocumentCommandBase(Document* document, QString label, QUndoCommand* parent = nullptr) :
        QUndoCommand{std::move(label), parent}, mDocument{document} {}

protected:
    Document* document() const { return mDocument; }
    Emitter* emitter(s32 setIndex, s32 emitterIndex) { return mDocument->emitterMutable(setIndex, emitterIndex); }
    EmitterSet* emitterSet(s32 setIndex) { return mDocument->emitterSetMutable(setIndex); }

    void setProjectName(const QString& newName) { mDocument->dataMutable().setName(newName); }

    void notifyEmitterChanged(s32 setIndex, s32 emitterIndex) { mDocument->notifyEmitterChanged(setIndex, emitterIndex); }
    void notifyEmitterSetChanged(s32 setIndex) { mDocument->notifyEmitterSetChanged(setIndex); }
    void notifyProjectChanged() { mDocument->notifyProjectChanged(); }

private:
    Document* mDocument;
};


// ========================================================================== //


template<typename T>
class SetEmitterPropertyCommand final : public DocumentCommandBase {
public:
    using Getter = std::function<T(const Emitter&)>;
    using Setter = std::function<void(Emitter&, const T&)>;

    SetEmitterPropertyCommand(Document* document, s32 setIndex, s32 emitterIndex, QString label,
        QString key, Getter getter, Setter setter, const T& newValue, QUndoCommand* parent = nullptr) :
        DocumentCommandBase{document, std::move(label), parent},
        mSetIndex{setIndex},
        mEmitterIndex{emitterIndex},
        mPropertyKey{std::move(key)},
        mGetter{std::move(getter)},
        mSetter{std::move(setter)},
        mNewValue{newValue},
        mId{static_cast<s32>(qHash(mPropertyKey))}
    {
        auto& emitter = getEmitter();
        mOldValue = mGetter(emitter);

        if (mOldValue == mNewValue) {
            setObsolete(true);
        }
    }

    s32 id() const override {
        return mId;
    }

    bool mergeWith(const QUndoCommand* other) override {
        if (other->id() != id()) {
            return false;
        }

        auto otherCmd = static_cast<const SetEmitterPropertyCommand*>(other);

        if (document() != otherCmd->document() || mSetIndex != otherCmd->mSetIndex ||
            mEmitterIndex != otherCmd->mEmitterIndex || mPropertyKey != otherCmd->mPropertyKey) {
            return false;
        }

        mNewValue = otherCmd->mNewValue;
        return true;
    }

    void undo() override { apply(mOldValue); }
    void redo() override { apply(mNewValue); }

private:
    Emitter& getEmitter();
    void apply(const T& value);

private:
    s32 mSetIndex{};
    s32 mEmitterIndex{};

    QString mPropertyKey{};

    Getter mGetter{};
    Setter mSetter{};

    T mOldValue{};
    T mNewValue{};

    const s32 mId{};
};


// ========================================================================== //


template <typename T>
Emitter& SetEmitterPropertyCommand<T>::getEmitter() {
    return *emitter(mSetIndex, mEmitterIndex);
}

template <typename T>
void SetEmitterPropertyCommand<T>::apply(const T& value) {
    auto& emitter = getEmitter();
    mSetter(emitter, value);
    notifyEmitterChanged(mSetIndex, mEmitterIndex);
}


// ========================================================================== //


class RenameProjectNameCommand final : public DocumentCommandBase {
public:
    RenameProjectNameCommand(Document* document, QString newName, QUndoCommand* parent = nullptr) :
        DocumentCommandBase{document, std::move("Rename Project"), parent}, mNewName{std::move(newName)} {
        mOldName = document->projectName();

        if (mOldName == mNewName) {
            setObsolete(true);
        }
    }

    s32 id() const override {
        return mId;
    }

    bool mergeWith(const QUndoCommand* other) override {
        if (other->id() != id()) {
            return false;
        }

        auto otherCmd = static_cast<const RenameProjectNameCommand*>(other);

        if (document() != otherCmd->document()) {
            return false;
        }

        mNewName = otherCmd->mNewName;
        return true;
    }

    void undo() override { apply(mOldName); }
    void redo() override { apply(mNewName); }

private:
    void apply(const QString& value) {
        setProjectName(value);
        notifyProjectChanged();
    };

private:
    QString mOldName{};
    QString mNewName{};

    const s32 mId{static_cast<s32>(qHash("RenameProject"))};
};



// ========================================================================== //


} // namespace Ptcl
