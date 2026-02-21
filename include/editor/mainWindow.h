#pragma once

#include "ptcl/ptclDocument.h"
#include "editor/emitterSetWidget.h"
#include "editor/emitterWidget/emitterWidget.h"
#include "editor/ptclListWidget.h"
#include "editor/textureListWidget.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QScrollArea>
#include <QSplitter>
#include <QUndoView>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


class MainWindow final : public QMainWindow {
    Q_OBJECT

private:
    enum class PropertiesView {
        EmitterSet,
        Emitter,
        EmitterChild,
        EmitterFlux,
        EmitterField
    };

public:
    MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) final;
    void dragEnterEvent(QDragEnterEvent* event) final;
    void dropEvent(QDropEvent* event) final;

private slots:
    void openFile();
    void saveFile();
    void saveFileAs();

    void openRecentFile();

private:
    void updateRecentFileList();
    void loadPtclRes(const QString& path);

    void setupUi();
    void setupConnections();
    void setupMenus();

    void setPropertiesView(PropertiesView view);
    void updatePropertiesStatus();

    void updateWindowTitle();
    void updateStatusBar();
    void setDirty(bool dirty);

    void bindUndoStack();

private:
    std::unique_ptr<Ptcl::Document> mDocument{};
    Ptcl::Selection mSelection{};

    QAction mOpenAction{};
    QAction mSaveAction{};
    QAction mSaveAsAction{};
    std::vector<QAction*> mRecentFileActions{};

    QAction* mUndoAction{nullptr};
    QAction* mRedoAction{nullptr};

    QMenu mFileMenu{};
    QMenu mRecentFilesMenu{};
    QMenu mEditMenu{};

    QSplitter* mTopSplitter{nullptr};
    QSplitter* mBottomSplitter{nullptr};

    QLineEdit mProjNameLineEdit{};

    QStackedWidget* mPropertiesStack{nullptr};
    QGroupBox mPropertiesGroup{};

    QUndoView mUndoView{};

    PtclEditor::PtclList mPtclList{};
    PtclEditor::EmitterWidget mEmitterWidget{};
    PtclEditor::EmitterSetWidget mEmitterSetWidget{};
    PtclEditor::TextureListWidget mTextureWidget{};

    PropertiesView mCurPropertiesView{PropertiesView::EmitterSet};

    QLabel* mStatusLabel{nullptr};
    bool mHasUnsavedChanges{false};
};


// ========================================================================== //


} // namespace PtclEditor
