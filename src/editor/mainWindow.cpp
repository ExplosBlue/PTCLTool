#include "editor/mainWindow.h"
#include "util/settingsUtil.h"

#include <QDataStream>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QMimeData>
#include <QStandardPaths>


namespace PtclEditor {


// ========================================================================== //


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent), mPtclList(this), mEmitterSetWidget(this), mTextureWidget(this) {
    setupUi();
    updateRecentFileList();
}

void MainWindow::setupUi() {
    // MainWindow
    setAcceptDrops(true);

    // Top Splitter
    mTopSplitter = new QSplitter(Qt::Horizontal, this);
    mTopSplitter->addWidget(&mPtclList);
    mTopSplitter->addWidget(&mEmitterSetWidget);
    mTopSplitter->setSizes({1, 9999});

    // Bottom Splitter
    mBottomSplitter = new QSplitter(Qt::Vertical, this);
    mBottomSplitter->addWidget(mTopSplitter);
    mBottomSplitter->addWidget(&mTextureWidget);

    // Ptcl List
    mPtclList.setEnabled(false);
    mPtclList.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    connect(&mPtclList, &PtclList::selectedEmitterSetChanged, this, &MainWindow::selectedEmitterSetChanged);
    connect(&mPtclList, &PtclList::selectedEmitterChanged, this, &MainWindow::selectedEmitterChanged);

    // EmitterSet Widget
    mEmitterSetWidget.setEnabled(false);
    mEmitterSetWidget.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(&mEmitterSetWidget, &EmitterSetWidget::textureUpdated, this, [this](s32 oldIndex, s32 newIndex) {
        if (oldIndex >= 0) { mTextureWidget.updateItemAt(oldIndex); }
        if (newIndex >= 0) { mTextureWidget.updateItemAt(newIndex); }
    });

    connect(&mEmitterSetWidget, &EmitterSetWidget::nameUpdated, this, [=, this](const QString& name) {
        mPtclList.refresh();
    });

    connect(&mEmitterSetWidget, &EmitterSetWidget::emitterAdded, this, [=, this]() {
        // TODO: this should insert a singluar list element instead of refreshing everything
        mPtclList.refresh();
    });

    connect(&mEmitterSetWidget, &EmitterSetWidget::emitterRemoved, this, [=, this]() {
        // TODO: this should remove a singluar list element instead of refreshing everything
        mPtclList.refresh();
    });

    connect(&mEmitterSetWidget, &EmitterSetWidget::emitterNameUpdated, this, [=, this]() {
        // TODO: this should update a singluar list element instead of refreshing everything
        mPtclList.refresh();
    });

    // Texture Widget
    mTextureWidget.setEnabled(false);

    setCentralWidget(mBottomSplitter);
    setupMenus();

    auto& settings = SettingsUtil::SettingsMgr::instance();

    restoreGeometry(settings.windowGeometry());
    restoreState(settings.windowState());
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

void MainWindow::closeEvent(QCloseEvent* event) {
    auto& settings = SettingsUtil::SettingsMgr::instance();

    settings.setWindowGeometry(saveGeometry());
    settings.setWindowState(saveState());

    QMainWindow::closeEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }

    QMainWindow::dragEnterEvent(event);
}

void MainWindow::dropEvent(QDropEvent* event) {
    const QList<QUrl> urls = event->mimeData()->urls();
    for (const auto& url : urls) {
        auto localPath = url.toLocalFile();

        QFileInfo fileInfo(localPath);
        if (fileInfo.exists()) {
            loadPtclRes(localPath);
        }
    }
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
    auto* action = qobject_cast<QAction*>(sender());

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
    mEmitterSetWidget.clear();
    mPtclList.setEnabled(false);
    mTextureWidget.setEnabled(false);

    mPtclRes = std::make_unique<Ptcl::PtclRes>();
    if (!mPtclRes->load(path)) {
        mPtclRes.reset();
        return;
    }

    SettingsUtil::SettingsMgr::instance().addRecentFile(path);
    SettingsUtil::SettingsMgr::instance().setLastOpenPath(QFileInfo(path).absolutePath());
    updateRecentFileList();

    mPtclList.setPtclRes(mPtclRes.get());
    mTextureWidget.setTextures(&mPtclRes->textures());
    mEmitterSetWidget.setTextureList(mPtclRes->textures());

    const auto& sets = mPtclRes->getEmitterSets();
    if (!sets.empty()) {
        selectedEmitterSetChanged(0);
    }

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

void MainWindow::selectedEmitterChanged(u32 setIndex, u32 emitterIndex) {
    mCurEmitterSetIdx = setIndex;
    auto &emitterSet = mPtclRes->getEmitterSets()[setIndex];

    mEmitterSetWidget.setEmitterSet(emitterSet.get());

    if (!mEmitterSetWidget.isEnabled()) {
        mEmitterSetWidget.setEnabled(true);
    }

    mEmitterSetWidget.setEmitterTab(emitterIndex);
}


// ========================================================================== //


} // namespace PtclEditor
