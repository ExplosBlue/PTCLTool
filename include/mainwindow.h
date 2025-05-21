#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QLabel>
#include <QComboBox>

#include "ptclList.h"

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
    void selectedEmitterChanged(u32 index);

private:
    Ui::MainWindow* ui;

    std::unique_ptr<Ptcl::PtclRes> mPtclRes;
    std::unique_ptr<PtclList> mPtclList;

    QVBoxLayout mImageInfoLayout;
    QComboBox mEmitterPicker;
    QLabel mImageLabel; // TODO: replace with graphics view
    QLabel mImageInfoLabel; // TODO: replace with graphics view

    QWidget mCenteralWidget;
    QHBoxLayout mMainLayout;

    u32 mCurEmitterSetIdx;
};
#endif // MAINWINDOW_H
