#include "editor/emitterList/emitterListContextMenu.h"
#include "editor/emitterList/emitterListController.h"
#include "util/dialogUtil.h"

#include <QMessageBox>


namespace PtclTool {


// ========================================================================== //


EmitterListContextMenu::EmitterListContextMenu(QWidget* parent) :
    QMenu{parent} {}

void EmitterListContextMenu::setDocument(Ptcl::Document* document) {
    mDocument = document;
}

void EmitterListContextMenu::setListController(EmitterListController* controller) {
    mListController = controller;
}

void EmitterListContextMenu::showForItem(const QPoint& globalPos, s32 setIndex, s32 emitterIndex, NodeType type, QStandardItem* item) {
    clear();

    addAddActions(type, item);
    addImportActions(type, setIndex);

    addSeparator();

    addRemoveAction(type, item);

    addSeparator();

    addDuplicateAction(type, item);
    addExportActions(type, setIndex, emitterIndex);
    addCopyPasteActions(type, item);

    exec(globalPos);
}

void EmitterListContextMenu::addAddActions(NodeType type, QStandardItem* item) {
    addAction("Add Emitter Set", this, [this, item] {
        mListController->addEmitterSet(item);
    });

    if (type == NodeType::EmitterSet || type == NodeType::Emitter) {
        addAction("Add Emitter", this, [this, item] {
            mListController->addEmitter(item);
        });
    }
}

void EmitterListContextMenu::addImportActions(NodeType type, s32 setIndex) {
    addAction("Import EmitterSet", this, [this] {
        const QString filePath = DialogUtil::getOpenFileName(
            this,
            "Import EmitterSet",
            SettingsUtil::PathType::ImportEmitterSet,
            {FileKind::EmitterSet}
        );

        if (filePath.isEmpty()) {
            return;
        }

        if (!mListController->importEmitterSet(filePath)) {
            QMessageBox::warning(this, "Import EmitterSet", "Failed to import emitter set. The source project textures could not be found.");
        }
    });

    if (type == NodeType::EmitterSet) {
        const s32 sourceSetIndex = setIndex;
        addAction("Import Emitter", this, [this, sourceSetIndex] {
            const QString filePath = DialogUtil::getOpenFileName(
                this,
                "Import Emitter",
                SettingsUtil::PathType::ImportEmitter,
                {FileKind::Emitter}
            );

            if (filePath.isEmpty()) {
                return;
            }

            if (!mListController->importEmitter(sourceSetIndex, filePath)) {
                QMessageBox::warning(this, "Import Emitter", "Failed to import emitter. The source project textures could not be found.");
            }
        });
    }
}

void EmitterListContextMenu::addRemoveAction(NodeType type, QStandardItem* item) {
    if (type != NodeType::EmitterSet && type != NodeType::Emitter) {
        return;
    }

    bool canRemove = false;
    if (type == NodeType::EmitterSet) {
        canRemove = mListController->model()->rowCount() > 1;
    } else {
        const auto* setItem = item->parent();
        if (setItem) {
            canRemove = setItem->rowCount() > 1;
        }
    }

    auto* removeAct = addAction("Remove", this, [this, item] {
        mListController->removeItem(item);
    });
    removeAct->setEnabled(canRemove);
}

void EmitterListContextMenu::addDuplicateAction(NodeType type, QStandardItem* item) {
    if (type != NodeType::EmitterSet && type != NodeType::Emitter) {
        return;
    }

    addAction("Duplicate", this, [this, item] {
        mListController->duplicateItem(item);
    });
}

void EmitterListContextMenu::addExportActions(NodeType type, s32 setIndex, s32 emitterIndex) {
    if (type == NodeType::Emitter) {
        addAction("Export Emitter", this, [this, setIndex, emitterIndex] {
            const auto* emitter = mDocument->emitter(setIndex, emitterIndex);
            if (!emitter) {
                return;
            }

            const QString defaultName = FileUtil::ensureExtention(emitter->name(), FileKind::Emitter);
            const QString filePath = DialogUtil::getSaveFileName(
                this,
                "Export Emitter",
                SettingsUtil::PathType::ExportEmitter,
                {FileKind::Emitter},
                defaultName
            );

            if (filePath.isEmpty()) {
                return;
            }

            mListController->exportEmitter(setIndex, emitterIndex, filePath);
        });
    } else if (type == NodeType::EmitterSet) {
        addAction("Export EmitterSet", this, [this, setIndex] {
            const auto* emitterSet = mDocument->emitterSet(setIndex);
            if (!emitterSet) {
                return;
            }

            const QString defaultName = FileUtil::ensureExtention(emitterSet->name(), FileKind::EmitterSet);
            const QString filePath = DialogUtil::getSaveFileName(
                this,
                "Export EmitterSet",
                SettingsUtil::PathType::ExportEmitterSet,
                {FileKind::EmitterSet},
                defaultName
            );

            if (filePath.isEmpty()) {
                return;
            }

            mListController->exportEmitterSet(setIndex, filePath);
        });
    }
}

void EmitterListContextMenu::addCopyPasteActions(NodeType type, QStandardItem* item) {
    if (type == NodeType::EmitterSet || type == NodeType::Emitter) {
        addAction("Copy", this, [this, item] {
            mListController->copyItem(item);
        });
    }

    auto* pasteAct = addAction("Paste", this, [this, item] {
        mListController->pasteItem(item);
    });
    pasteAct->setEnabled(mListController->canPaste());
}


// ========================================================================== //


} // namespace PtclTool
