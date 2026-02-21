#pragma once

#include "ptcl/ptclEmitter.h"
#include "ptcl/ptclDocument.h"


#include <QUndoCommand>


namespace Ptcl {


// ========================================================================== //


template<typename T>
class SetEmitterPropertyCommand final : public QUndoCommand {
public:
    using Getter = std::function<T(const Emitter&)>;
    using Setter = std::function<void(Emitter&, const T&)>;

    SetEmitterPropertyCommand(Document* document, s32 setIndex, s32 emitterIndex, QString label, QString key, Getter getter, Setter setter, const T& newValue, QUndoCommand* parent = nullptr) :
        QUndoCommand{std::move(label), parent}, mDocument{document}, mSetIndex{setIndex}, mEmitterIndex{emitterIndex}, mPropertyKey{std::move(key)}, mGetter{std::move(getter)}, mSetter{std::move(setter)}, mNewValue{newValue} {
        auto& emitter = getEmitter();
        mOldValue = mGetter(emitter);

        if (mOldValue == mNewValue) {
            setObsolete(true);
        }
    }

    s32 id() const override {
        return qHash(mPropertyKey);
    }

    bool mergeWith(const QUndoCommand* other) override {
        auto otherCmd = dynamic_cast<const SetEmitterPropertyCommand*>(other);
        if (!otherCmd) {
            return false;
        }

        if (mDocument != otherCmd->mDocument || mSetIndex != otherCmd->mSetIndex ||
            mEmitterIndex != otherCmd->mEmitterIndex || mPropertyKey != otherCmd->mPropertyKey) {
            return false;
        }

        mNewValue = otherCmd->mNewValue;
        return true;
    }

    void undo() override { apply(mOldValue); }
    void redo() override { apply(mNewValue); }

private:
    Emitter& getEmitter() const;
    void apply(const T& value);

private:
    Document* mDocument{};
    s32 mSetIndex{};
    s32 mEmitterIndex{};

    QString mPropertyKey{};

    Getter mGetter{};
    Setter mSetter{};

    T mOldValue{};
    T mNewValue{};
};


// ========================================================================== //


template <typename T>
Emitter& SetEmitterPropertyCommand<T>::getEmitter() const {
    return *mDocument->emitter(mSetIndex, mEmitterIndex);
}

template <typename T>
void SetEmitterPropertyCommand<T>::apply(const T& value) {
    auto& emitter = getEmitter();
    mSetter(emitter, value);
    mDocument->notifyEmitterChanged(mSetIndex, mEmitterIndex);
}


// ========================================================================== //


} // namespace Ptcl
