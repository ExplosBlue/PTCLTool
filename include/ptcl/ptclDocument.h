#pragma once

#include "ptcl/ptcl.h"

#include <QObject>


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


class Document final : public QObject {
    Q_OBJECT
public:
    explicit Document(QObject* parent = nullptr);

    bool load(const QString& filePath);
    bool save(const QString& filePath);

    PtclRes& data() { return mData; }
    const PtclRes& data() const { return mData; }

    EmitterSet* emitterSet(s32 index) { return mData.emitterSet(index); }
    const EmitterSet* emitterSet(s32 index) const { return mData.emitterSet(index); }

    Emitter* emitter(s32 setIndex, s32 emitterIndex) { return mData.emitter(setIndex, emitterIndex); }
    const Emitter* emitter(s32 setIndex, s32 emitterIndex) const { return mData.emitter(setIndex, emitterIndex); }

    TextureList& textures() { return mData.textures(); }
    const TextureList& textures() const { return mData.textures(); }

    bool isModified() const { return mModified; }
    QString filePath() const  { return mFilePath; }

private:
    PtclRes mData{};
    QString mFilePath{};
    bool mModified{false};
};


// ========================================================================== //


} // namespace Ptcl
