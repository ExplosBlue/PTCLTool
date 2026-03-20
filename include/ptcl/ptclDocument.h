#pragma once

#include "ptcl/ptcl.h"

#include <QObject>
#include <QUndoStack>


namespace Ptcl {


// ========================================================================== //


class Selection final : public QObject {
    Q_OBJECT
public:
    enum class Type {
        None,
        EmitterSet,
        Emitter,
        EmitterChild,
        EmitterFlux,
        EmitterField
    };

public:
    explicit Selection(QObject* parent = nullptr);

    void set(s32 setIndex, s32 emitterIndex, Type type);

    s32 emitterSetIndex() const { return mSetIndex; }
    s32 emitterIndex() const { return mEmitterIndex; }
    Type type() const { return mType; }

signals:
    void selectionChanged(s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type type);

private:
    s32 mSetIndex{-1};
    s32 mEmitterIndex{-1};
    Type mType{Selection::Type::None};
};


// ========================================================================== //


class DocumentCommandBase;

template<typename T>
class SetEmitterPropertyCommand;

template<typename T>
class SetEmitterSetPropertyCommand;

// ========================================================================== //


class Document final : public QObject {
    Q_OBJECT
public:
    explicit Document(QObject* parent = nullptr);

    // TODO: Remove this
    EmitterSet* emitterSet(s32 index) { return mData.emitterSet(index); }
    // TODO: Remove this
    TextureList& textures() { return mData.textures(); }
    // TODO: Remove this
    PtclRes& data() { return mData; }

    bool load(const QString& filePath);
    bool save(const QString& filePath);

    const Emitter* emitter(s32 setIndex, s32 emitterIndex) const { return mData.emitter(setIndex, emitterIndex); }
    const EmitterSet* emitterSet(s32 index) const { return mData.emitterSet(index); }

    const EmitterSetList& emitterSets() const { return mData.getEmitterSets(); }
    const TextureList& textures() const { return mData.textures(); }

    const QString& projectName() const { return mData.name(); }
    void setProjectName(const QString& name);

    bool isModified() const { return mModified; }
    QString filePath() const  { return mFilePath; }

    QUndoStack* undoStack() { return &mUndoStack; }

    template<typename T, typename Getter, typename Setter>
    void setEmitterProperty(s32 setIndex, s32 emitterIndex, QString label, QString key, Getter getter, Setter setter, const T& value) {
        mUndoStack.push(new SetEmitterPropertyCommand<T>(this, setIndex, emitterIndex, label, key, getter, setter, value));
    }

    template<typename T, typename Getter, typename Setter>
    void setEmitterSetProperty(s32 setIndex, QString label, QString key, Getter getter, Setter setter, const T& value) {
        mUndoStack.push(new SetEmitterSetPropertyCommand<T>(this, setIndex, label, key, getter, setter, value));
    }

    s32 emitterCount(s32 setIndex) const { return mData.emitterCount(setIndex); }
    s32 emitterSetCount() const { return mData.emitterSetCount(); }

    void addEmitter(s32 setIndex, std::unique_ptr<Emitter> emitter = nullptr);
    void removeEmitter(s32 setIndex, s32 emitterIndex);

    void addEmitterSet(std::unique_ptr<EmitterSet> emitterSet = nullptr);
    void removeEmitterSet(s32 setIndex);

private:
    TextureList& texturesMutable() { return mData.textures(); }
    Emitter* emitterMutable(s32 setIndex, s32 emitterIndex) { return mData.emitter(setIndex, emitterIndex); }
    EmitterSet* emitterSetMutable(s32 index) { return mData.emitterSet(index); }
    EmitterSetList& emitterSetsMutable() { return mData.getEmitterSets(); }

    PtclRes& dataMutable() { return mData; }

    void notifyEmitterChanged(s32 setIndex, s32 emitterIndex) { emit emitterChanged(setIndex, emitterIndex); }
    void notifyEmitterSetChanged(s32 setIndex) { emit emitterSetChanged(setIndex); }
    void notifyProjectChanged() { emit projectChanged(); }

    void notifyEmitterAdded(s32 setIndex, s32 emitterIndex) { emit emitterAdded(setIndex, emitterIndex); }
    void notifyEmitterRemoved(s32 setIndex, s32 emitterIndex) { emit emitterRemoved(setIndex, emitterIndex); }

    void notifyEmitterSetAdded(s32 setIndex) { emit emitterSetAdded(setIndex); }
    void notifyEmitterSetRemoved(s32 setIndex) { emit emitterSetRemoved(setIndex); }

    friend class DocumentCommandBase;

signals:
    void emitterChanged(s32 setIndex, s32 emitterIndex);
    void emitterSetChanged(s32 setIndex);
    void projectChanged();

    void emitterAdded(s32 setIndex, s32 emitterIndex);
    void emitterRemoved(s32 setIndex, s32 emitterIndex);

    void emitterSetAdded(s32 setIndex);
    void emitterSetRemoved(s32 setIndex);

private:
    PtclRes mData{};
    QString mFilePath{};
    QUndoStack mUndoStack{};

    bool mModified{false};
};


// ========================================================================== //


} // namespace Ptcl

#include "ptcl/ptclCommand.h"
