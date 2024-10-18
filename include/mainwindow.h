#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLabel>

#include "ptclList.h"
#include "emitterSetWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() final;

    void LoadData();

public slots:
    void selectedEmitterSetChanged(u32 index);

private:
    Ui::MainWindow* ui;

    std::unique_ptr<Ptcl::PtclRes> mPtclRes;
    std::unique_ptr<PtclList> mPtclList;
    std::unique_ptr<EmitterSetWidget> mEmitterSetWidget;

    // Todo: this shouldn't be in main window
    QLineEdit mNameEdit;

    QWidget* mCenteralWidget;
    QHBoxLayout mMainLayout;

    u32 mCurEmitterSetIdx;
};
#endif // MAINWINDOW_H
