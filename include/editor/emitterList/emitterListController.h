#pragma once

#include "editor/emitterList/emitterListTypes.h"
#include "ptcl/ptclDocument.h"

#include <QObject>
#include <QStandardItemModel>

#include <memory>


namespace PtclTool {


// ========================================================================== //


class EmitterListController : public QObject {
    Q_OBJECT
public:
    explicit EmitterListController(QObject* parent = nullptr);

    void setDocument(Ptcl::Document* document);
    void setSelection(Ptcl::Selection* selection);

    QStandardItemModel* model();
    const QStandardItemModel* model() const;

    void populate();

    QStandardItem* findItem(s32 setIndex, s32 emitterIndex, Ptcl::Selection::Type type) const;

    bool canPaste() const;

    void addEmitterSet(QStandardItem* contextItem = nullptr);
    void addEmitter(QStandardItem* contextItem = nullptr);
    void removeItem(QStandardItem* contextItem = nullptr);
    void duplicateItem(QStandardItem* contextItem = nullptr);
    void copyItem(QStandardItem* contextItem = nullptr);
    void pasteItem(QStandardItem* contextItem = nullptr);

    bool importEmitterSet(const QString& filePath);
    bool importEmitter(s32 setIndex, const QString& filePath);
    bool exportEmitter(s32 setIndex, s32 emitterIndex, const QString& filePath);
    bool exportEmitterSet(s32 setIndex, const QString& filePath);

signals:
    void contentChanged();

private:
    static QString itemLabel(s32 index, const QString& name);
    static QStandardItem* makeNode(const QString& label, NodeType type, s32 setIndex, s32 emitterIndex = -1);

    void insertEmitterSetNode(s32 setIndex);
    void insertEmitterNode(QStandardItem* setItem, s32 setIndex, s32 emitterIndex);
    void addComplexNodes(QStandardItem* emitterItem, s32 setIndex, s32 emitterIndex);
    void ensureComplexNode(QStandardItem* emitterItem, NodeType type, const QString& label, s32 setIndex, s32 emitterIndex, bool enabled);
    static QStandardItem* findChildByType(QStandardItem* parent, NodeType type);

    void removeEmitter(QStandardItem* setItem, QStandardItem* emitterItem);
    void removeEmitterSet(QStandardItem* setItem);
    void duplicateEmitterSet(QStandardItem* contextItem);
    void duplicateEmitter(QStandardItem* contextItem);

    void reindexEmitters(QStandardItem* setItem, s32 setIndex);
    void reindexEmitterSets();

    void selectNearestValidEmitter(s32 setIndex, s32 preferredEmitter);
    void selectNearestValidEmitterSet(s32 preferredSet);

    void updateEmitter(s32 setIndex, s32 emitterIndex);
    void updateEmitterName(s32 setIndex, s32 emitterIndex);
    void updateEmitterSetName(s32 setIndex);

private:
    QStandardItemModel mListModel{};

    Ptcl::Document* mDocument{nullptr};
    Ptcl::Selection* mSelection{nullptr};

    std::unique_ptr<Ptcl::EmitterSet> mClipboardSet{};
    std::unique_ptr<Ptcl::Emitter> mClipboardEmitter{};
};


// ========================================================================== //


} // namespace PtclTool
