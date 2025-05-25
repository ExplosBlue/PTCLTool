#include "editor/mainWindow.h"
#include "ui/ui_mainwindow.h"

#include "util/settingsUtil.h"

#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QHBoxLayout>
#include <QStandardPaths>

namespace PtclEditor {

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), mUi{new Ui::MainWindow} {
    mUi->setupUi(this);
    connect(mUi->ptclList, &PtclList::selectedIndexChanged, this, &MainWindow::selectedEmitterSetChanged);
}

void MainWindow::openFile() {
    QString basePath = SettingsUtil::SettingsMgr::instance().lastOpenPath();

    if (basePath.isEmpty()) {
        basePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    }

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

    SettingsUtil::SettingsMgr::instance().addRecentFile(filePath);
    SettingsUtil::SettingsMgr::instance().setLastOpenPath(QFileInfo(filePath).absolutePath());

    mPtclRes->getEmitterSets();

    mUi->ptclList->setPtclRes(mPtclRes.get());
    mUi->textureWidget->setTextures(&mPtclRes->textures());
}

void MainWindow::saveFile() {
    if (!mPtclRes) {
        return;
    }

    QString basePath = SettingsUtil::SettingsMgr::instance().lastSavePath();
    if (basePath.isEmpty()) {
        QString lastOpenPath = SettingsUtil::SettingsMgr::instance().lastOpenPath();
        if (!lastOpenPath.isEmpty()) {
            basePath = lastOpenPath;
        } else {
            basePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
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

    SettingsUtil::SettingsMgr::instance().addRecentFile(filePath);
    SettingsUtil::SettingsMgr::instance().setLastSavePath(QFileInfo(filePath).absolutePath());
}

void MainWindow::selectedEmitterSetChanged(u32 index) {
    mCurEmitterSetIdx = index;
    auto &emitterSet = mPtclRes->getEmitterSets()[index];

    mUi->emitterSetWidget->setEmitterSet(emitterSet.get());
}


} // namespace PtclEditor
