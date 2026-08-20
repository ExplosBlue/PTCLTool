#pragma once

#include "editor/components/collapsiblePanel.h"
#include "editor/components/panelSplitter.h"
#include "ptcl/ptclDocument.h"
#include "editor/inspector/inspectorPanel.h"
#include "editor/ptclListWidget.h"
#include "editor/texture/textureListWidget.h"

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

#include <memory>
#include <vector>


namespace PtclEditor {


// ========================================================================== //


class MainWindow final : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() final;

protected:
    void closeEvent(QCloseEvent* event) final;
    void dragEnterEvent(QDragEnterEvent* event) final;
    void dropEvent(QDropEvent* event) final;

private slots:
    void openFile();
    void saveFile();
    void saveFileAs();

    void exportProject();

    void openRecentFile();

private:
    void updateRecentFileList();
    void loadDocument(const QString& path);

    void setupUi();
    void setupMenus();

    void applyIcons();

    void updateWindowTitle();
    void updateStatusBar();

    void saveSplitterState();
    void restoreSplitterState();

    void bindUndoStack();

    void showFileNotFoundDialog(const QString& filePath);
    void showOpenErrorDialog(const QString& filePath);
    void showSanitizeWarningDialog(const QString& filePath, const Ptcl::PtclSanitizeReport& report);
    void showNoDocumentWarningDialog();

    void dropImage(const QString& filePath);

private:
    std::unique_ptr<Ptcl::Document> mDocument{};
    Ptcl::Selection mSelection{};

    QAction mOpenAction{};
    QAction mSaveAction{};
    QAction mSaveAsAction{};
    QAction mExportAction{};
    std::vector<QAction*> mRecentFileActions{};

    QAction* mUndoAction{nullptr};
    QAction* mRedoAction{nullptr};

    QMenu mFileMenu{};
    QMenu mRecentFilesMenu{};
    QMenu mEditMenu{};
    QMenu mViewMenu{};

    QAction* mHistoryAction{nullptr};
    QAction* mTextureAction{nullptr};

    PanelSplitter* mRootSplitter{nullptr};
    PanelSplitter* mLeftSplitter{nullptr};
    PanelSplitter* mRightSplitter{nullptr};

    CollapsiblePanel mHistoryPanel{"History"};
    CollapsiblePanel mTexturePanel{"Textures"};

    QUndoView mUndoView{};

    PtclEditor::PtclList mPtclList{};
    PtclEditor::InspectorPanel mInspector{};
    PtclEditor::TextureListWidget mTextureWidget{};

    QLabel* mStatusLabel{nullptr};
};


// ========================================================================== //


} // namespace PtclEditor
