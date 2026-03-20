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

    EmitterList& emitterList(s32 setIndex) { return mDocument->emitterSetMutable(setIndex)->emitters(); }

    void setProjectName(const QString& newName) { mDocument->dataMutable().setName(newName); }

    void notifyEmitterChanged(s32 setIndex, s32 emitterIndex) { mDocument->notifyEmitterChanged(setIndex, emitterIndex); }
    void notifyEmitterSetChanged(s32 setIndex) { mDocument->notifyEmitterSetChanged(setIndex); }
    void notifyProjectChanged() { mDocument->notifyProjectChanged(); }
    void notifyEmitterAdded(s32 setIndex, s32 emitterIndex) { mDocument->notifyEmitterAdded(setIndex, emitterIndex); }
    void notifyEmitterRemoved(s32 setIndex, s32 emitterIndex) { mDocument->notifyEmitterRemoved(setIndex, emitterIndex); }

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
    auto& emitterSet = getEmitter();
    mSetter(emitterSet, value);
    notifyEmitterChanged(mSetIndex, mEmitterIndex);
}


// ========================================================================== //


template<typename T>
class SetEmitterSetPropertyCommand final : public DocumentCommandBase {
public:
    using Getter = std::function<T(const EmitterSet&)>;
    using Setter = std::function<void(EmitterSet&, const T&)>;

    SetEmitterSetPropertyCommand(Document* document, s32 setIndex, QString label,
        QString key, Getter getter, Setter setter, const T& newValue, QUndoCommand* parent = nullptr) :
        DocumentCommandBase{document, std::move(label), parent},
        mSetIndex{setIndex},
        mPropertyKey{std::move(key)},
        mGetter{std::move(getter)},
        mSetter{std::move(setter)},
        mNewValue{newValue},
        mId{static_cast<s32>(qHash(mPropertyKey))}
    {
        auto& emitterSet = getEmitterSet();
        mOldValue = mGetter(emitterSet);

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

        auto otherCmd = static_cast<const SetEmitterSetPropertyCommand*>(other);

        if (document() != otherCmd->document() || mSetIndex != otherCmd->mSetIndex ||
            mPropertyKey != otherCmd->mPropertyKey) {
            return false;
        }

        mNewValue = otherCmd->mNewValue;
        return true;
    }

    void undo() override { apply(mOldValue); }
    void redo() override { apply(mNewValue); }

private:
    EmitterSet& getEmitterSet();
    void apply(const T& value);

private:
    s32 mSetIndex{};

    QString mPropertyKey{};

    Getter mGetter{};
    Setter mSetter{};

    T mOldValue{};
    T mNewValue{};

    const s32 mId{};
};


// ========================================================================== //


template <typename T>
EmitterSet& SetEmitterSetPropertyCommand<T>::getEmitterSet() {
    return *emitterSet(mSetIndex);
}

template <typename T>
void SetEmitterSetPropertyCommand<T>::apply(const T& value) {
    auto& emitterSet = getEmitterSet();
    mSetter(emitterSet, value);
    notifyEmitterSetChanged(mSetIndex);
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


class AddEmitterCommand final : public DocumentCommandBase {
public:
    AddEmitterCommand(Document* doc, s32 setIndex, std::unique_ptr<Emitter> emitter = nullptr, QUndoCommand* parent = nullptr) :
        DocumentCommandBase{doc, std::move("Add Emitter"), parent}, mSetIndex{setIndex} {

        if (emitter) {
            mNewEmitter = std::move(emitter);
        } else {
            auto newEmitterPtr = std::make_unique<Emitter>();
            newEmitterPtr->setName("New_Emitter_" + QString::number(document()->emitterCount(mSetIndex)));
            mNewEmitter = std::move(newEmitterPtr);
        }

    }

    s32 id() const override {
        return mId;
    }

    void undo() override {
        auto* set = emitterSet(mSetIndex);
        mNewEmitter = std::move(set->emitters()[mEmitterIndex]);
        set->removeEmitter(mEmitterIndex);
        notifyEmitterRemoved(mSetIndex, mEmitterIndex);
    }

    void redo() override {
        auto* set = emitterSet(mSetIndex);
        set->emitters().push_back(std::move(mNewEmitter));
        mEmitterIndex = set->emitterCount() - 1;
        notifyEmitterAdded(mSetIndex, mEmitterIndex);
    }

private:
    s32 mSetIndex{};
    s32 mEmitterIndex{0};
    std::unique_ptr<Emitter> mNewEmitter{};

    const s32 mId{static_cast<s32>(qHash("AddEmitter"))};
};


// ========================================================================== //


class RemoveEmitterCommand final : public DocumentCommandBase {
public:
    RemoveEmitterCommand(Document* doc, s32 setIndex, s32 emitterIndex, QUndoCommand* parent = nullptr) :
        DocumentCommandBase{doc, std::move("Remove Emitter"), parent}, mSetIndex{setIndex}, mEmitterIndex{emitterIndex} {

        auto* set = emitterSet(mSetIndex);
        if (mEmitterIndex < set->emitterCount()) {
            mRemovedEmitter = std::move(set->emitters()[mEmitterIndex]);
        }
    }

    s32 id() const override {
        return mId;
    }

    void undo() override {
        auto* set = emitterSet(mSetIndex);
        set->emitters().insert(set->emitters().begin() + mEmitterIndex, std::move(mRemovedEmitter));
        notifyEmitterAdded(mSetIndex, mEmitterIndex);
    }

    void redo() override {
        auto* set = emitterSet(mSetIndex);
        mRemovedEmitter = std::move(set->emitters()[mEmitterIndex]);
        set->removeEmitter(mEmitterIndex);
        notifyEmitterRemoved(mSetIndex, mEmitterIndex);
    }

private:
    s32 mSetIndex{};
    s32 mEmitterIndex{0};
    std::unique_ptr<Emitter> mRemovedEmitter{};

    const s32 mId{static_cast<s32>(qHash("RemoveEmitter"))};
};


// ========================================================================== //

} // namespace Ptcl
