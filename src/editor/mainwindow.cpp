#include "editor/mainwindow.h"
#include "ui/ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QHBoxLayout>

namespace PtclEditor {

// TODO: REMOVE THIS
static const QString basePath = "D://Explos//Home//Modding//3DS//Games//NSMB2//Rom//Gold Edition - USA//ExtractedRomFS//Effect";

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), mUi{new Ui::MainWindow} {

    mUi->setupUi(this);

    connect(mUi->ptclList, &PtclList::selectedIndexChanged, this, &MainWindow::selectedEmitterSetChanged);

    LoadData();
}

void MainWindow::LoadData() {

    QString filename = QFileDialog::getOpenFileName(this, "Select File", basePath);

    mPtclRes = std::make_unique<Ptcl::PtclRes>();
    mPtclRes->load(filename);

    mPtclRes->getEmitterSets();

    mUi->ptclList->setPtclRes(mPtclRes.get());
    mUi->textureWidget->setTextures(&mPtclRes->textures());
}

void MainWindow::selectedEmitterSetChanged(u32 index) {

    mCurEmitterSetIdx = index;
    auto &emitterSet = mPtclRes->getEmitterSets()[index];

    mUi->emitterSetWidget->setEmitterSet(emitterSet.get());
}

} // namespace PtclEditor
