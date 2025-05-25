#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLabel>
#include <QScrollArea>
#include <QMenu>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenuBar>

#include "typedefs.h"

#include "ptcl/ptcl.h"

#include "editor/ptclListWidget.h"
#include "editor/emitterSetWidget.h"
#include "editor/textureListWidget.h"

namespace PtclEditor {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

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

} // namespace PtclEditor

#endif // MAINWINDOW_H
