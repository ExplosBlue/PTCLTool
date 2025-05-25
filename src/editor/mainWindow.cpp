#include "editor/mainWindow.h"

#include "util/settingsUtil.h"

#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QStandardPaths>

namespace PtclEditor {

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    mPtclList(this),
    mEmitterSetWidget(this),
    mTextureWidget(this) {
    setupUi();
    updateRecentFileList();
}

void MainWindow::setupUi() {
    // Top Splitter
    mTopSplitter = new QSplitter(Qt::Horizontal, this);
    mTopSplitter->addWidget(&mPtclList);
    mTopSplitter->addWidget(&mEmitterSetWidget);

    // Bottom Splitter
    mBottomSplitter = new QSplitter(Qt::Vertical, this);
    mBottomSplitter->addWidget(mTopSplitter);
    mBottomSplitter->addWidget(&mTextureWidget);

    // Ptcl List
    mPtclList.setEnabled(false);
    connect(&mPtclList, &PtclList::selectedIndexChanged, this, &MainWindow::selectedEmitterSetChanged);

    // EmitterSet Widget
    mEmitterSetWidget.setEnabled(false);

    // Texture Widget
    mTextureWidget.setEnabled(false);

    setCentralWidget(mBottomSplitter);
    setupMenus();
}

void MainWindow::setupMenus() {
    // Open File Action
    mOpenAction.setText("Open File");
    mOpenAction.setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen));
    mOpenAction.setShortcut(QKeySequence::Open);
    connect(&mOpenAction, &QAction::triggered, this, &MainWindow::openFile);

    // Save File Action
    mSaveAction.setText("Save As");
    mSaveAction.setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSaveAs));
    mSaveAction.setShortcut(QKeySequence::SaveAs);
    connect(&mSaveAction, &QAction::triggered, this, &MainWindow::saveFile);

    // Recent Files Menu
    mRecentFilesMenu.setTitle("Recent Files");
    mRecentFilesMenu.setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpenRecent));

    // Recent Files Actions
    int maxRecentFiles = SettingsUtil::SettingsMgr::instance().maxRecentFiles();
    for (int i = 0; i < maxRecentFiles; ++i) {
        QAction* recentFileAction = mRecentFilesMenu.addAction("");
        recentFileAction->setVisible(false);
        connect(recentFileAction, &QAction::triggered, this, &MainWindow::openRecentFile);
        mRecentFileActions.push_back(recentFileAction);
    }

    // File Menu
    mFileMenu.setTitle("File");
    mFileMenu.addAction(&mOpenAction);
    mFileMenu.addAction(&mSaveAction);
    mFileMenu.addSeparator();
    mFileMenu.addMenu(&mRecentFilesMenu);

    // Menu Bar
    menuBar()->addMenu(&mFileMenu);
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
    loadPtclRes(filePath);
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
    updateRecentFileList();
}

void MainWindow::openRecentFile() {
    QAction* action = qobject_cast<QAction*>(sender());

    if (!action) {
        return;
    }

    QString filePath = action->data().toString();
    loadPtclRes(filePath);
}

void MainWindow::updateRecentFileList() {
    const auto recentFiles = SettingsUtil::SettingsMgr::instance().recentFiles();

    int maxRecentFiles = SettingsUtil::SettingsMgr::instance().maxRecentFiles();
    qsizetype numRecentFiles = qMin(recentFiles.size(), maxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        auto& file = recentFiles[i];
        auto& action = mRecentFileActions[i];

        QString text = QString("&%1").arg(QFileInfo(file).fileName());
        action->setText(text);
        action->setData(file);
        action->setVisible(true);
    }

    for (qsizetype i = numRecentFiles; i < maxRecentFiles; ++i) {
        mRecentFileActions[i]->setVisible(false);
    }

    mRecentFilesMenu.setEnabled(numRecentFiles > 0);
}

void MainWindow::loadPtclRes(const QString& path) {
    mPtclRes = std::make_unique<Ptcl::PtclRes>();
    if (!mPtclRes->load(path)) {
        mPtclRes.reset();
        return;
    }

    SettingsUtil::SettingsMgr::instance().addRecentFile(path);
    SettingsUtil::SettingsMgr::instance().setLastOpenPath(QFileInfo(path).absolutePath());
    updateRecentFileList();

    mPtclRes->getEmitterSets();

    mPtclList.setPtclRes(mPtclRes.get());
    mTextureWidget.setTextures(&mPtclRes->textures());

    mPtclList.setEnabled(true);
    mTextureWidget.setEnabled(true);
}

void MainWindow::selectedEmitterSetChanged(u32 index) {
    mCurEmitterSetIdx = index;
    auto &emitterSet = mPtclRes->getEmitterSets()[index];

    mEmitterSetWidget.setEmitterSet(emitterSet.get());

    if (!mEmitterSetWidget.isEnabled()) {
        mEmitterSetWidget.setEnabled(true);
    }
}


} // namespace PtclEditor
