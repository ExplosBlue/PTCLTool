#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLabel>
#include <QScrollArea>
#include <QMenu>

#include "typedefs.h"

#include "ptcl/ptcl.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

namespace PtclEditor {

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

public slots:
    void selectedEmitterSetChanged(u32 index);

    void openFile();
    void saveFile();

    void openRecentFile();

private:
    void updateRecentFileList();
    void loadPtclRes(const QString& path);

private:
    Ui::MainWindow* mUi;
    // std::unique_ptr<Ui::MainWindow> mUi;
    std::unique_ptr<Ptcl::PtclRes> mPtclRes;

    std::vector<QAction*> mRecentFileActions;

    u32 mCurEmitterSetIdx;

    QMenu mRecentFilesMenu;
};

} // namespace PtclEditor

#endif // MAINWINDOW_H
