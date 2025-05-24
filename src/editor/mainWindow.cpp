#include "editor/mainWindow.h"
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
}

void MainWindow::openFile() {
    QFileDialog openFileDialog(this, "Open File", basePath, "*.ptcl");

    if (openFileDialog.exec() == QFileDialog::DialogCode::Rejected) {
        return;
    }

    const auto& files = openFileDialog.selectedFiles();

    if (files.isEmpty()) {
        return;
    }

    auto filePath = files.first();
    mPtclRes = std::make_unique<Ptcl::PtclRes>();
    if (!mPtclRes->load(filePath)) {
        mPtclRes.reset();
        return;
    }

    mPtclRes->getEmitterSets();

    mUi->ptclList->setPtclRes(mPtclRes.get());
    mUi->textureWidget->setTextures(&mPtclRes->textures());
}

void MainWindow::saveFile() {
    if (!mPtclRes) {
        return;
    }

    QFileDialog saveFileDialog(this, "Save As", basePath, "*.ptcl");

    if(saveFileDialog.exec() == QFileDialog::DialogCode::Rejected) {
        return;
    }

    const auto& files = saveFileDialog.selectedFiles();

    if (files.isEmpty()) {
        return;
    }

    auto filePath = files.first();

    if (!filePath.endsWith(".ptcl")) {
        filePath.append(".ptcl");
    }

    mPtclRes->save(filePath);
}

void MainWindow::selectedEmitterSetChanged(u32 index) {

    mCurEmitterSetIdx = index;
    auto &emitterSet = mPtclRes->getEmitterSets()[index];

    mUi->emitterSetWidget->setEmitterSet(emitterSet.get());
}

} // namespace PtclEditor
