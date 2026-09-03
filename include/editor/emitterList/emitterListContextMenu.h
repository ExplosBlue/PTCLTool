#pragma once

#include "editor/emitterList/emitterListTypes.h"
#include "ptcl/ptclDocument.h"

#include <QMenu>
#include <QStandardItemModel>


namespace PtclTool {


// ========================================================================== //


class EmitterListController;

class EmitterListContextMenu : public QMenu {
    Q_OBJECT
public:
    explicit EmitterListContextMenu(QWidget* parent = nullptr);

    void setDocument(Ptcl::Document* document);
    void setListController(EmitterListController* controller);

    void showForItem(const QPoint& globalPos, s32 setIndex, s32 emitterIndex, NodeType type, QStandardItem* item);

private:
    void addAddActions(NodeType type, QStandardItem* item);
    void addImportActions(NodeType type, s32 setIndex);
    void addRemoveAction(NodeType type, QStandardItem* item);
    void addDuplicateAction(NodeType type, QStandardItem* item);
    void addExportActions(NodeType type, s32 setIndex, s32 emitterIndex);
    void addCopyPasteActions(NodeType type, QStandardItem* item);

private:
    Ptcl::Document* mDocument{nullptr};
    EmitterListController* mListController{nullptr};
};


// ========================================================================== //


} // namespace PtclTool
