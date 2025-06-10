#pragma once

#include "typedefs.h"
#include "ptcl/ptcl.h"
#include "editor/emitterSetWidget.h"
#include "editor/ptclListWidget.h"
#include "editor/textureListWidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) final;
    void dragEnterEvent(QDragEnterEvent* event) final;
    void dropEvent(QDropEvent* event) final;

private slots:
    void selectedEmitterSetChanged(u32 index);

    void openFile();
    void saveFile();

    void openRecentFile();

private:
    void updateRecentFileList();
    void loadPtclRes(const QString& path);

    void setupUi();
    void setupMenus();

private:
    std::unique_ptr<Ptcl::PtclRes> mPtclRes;

    u32 mCurEmitterSetIdx;

    QAction mOpenAction;
    QAction mSaveAction;
    std::vector<QAction*> mRecentFileActions;

    QMenu mFileMenu;
    QMenu mRecentFilesMenu;

    QSplitter* mTopSplitter;
    QSplitter* mBottomSplitter;

    PtclEditor::PtclList mPtclList;
    PtclEditor::EmitterSetWidget mEmitterSetWidget;
    PtclEditor::TextureListWidget mTextureWidget;
};


// ========================================================================== //


} // namespace PtclEditor
