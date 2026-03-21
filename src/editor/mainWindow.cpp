#include "editor/mainWindow.h"
#include "util/settingsUtil.h"

#include <QDataStream>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QFormLayout>
#include <QLabel>
#include <QMimeData>
#include <QMessageBox>
#include <QStandardPaths>
#include <QStatusBar>


namespace PtclEditor {


// ========================================================================== //


MainWindow::MainWindow(QWidget* parent) :
    QMainWindow{parent} {
    setupUi();
    updateRecentFileList();
}

void MainWindow::setupUi() {
    // MainWindow
    setAcceptDrops(true);

    // Top Splitter
    mTopSplitter = new QSplitter(Qt::Horizontal, this);
    mTopSplitter->addWidget(&mPtclList);
    mTopSplitter->addWidget(&mInspector);
    mTopSplitter->setStretchFactor(0, 0);
    mTopSplitter->setStretchFactor(1, 1);

    // Bottom Container
    auto* bottomContainer = new QWidget(this);
    auto* bottomLayout = new QHBoxLayout(bottomContainer);
    bottomLayout->addWidget(&mUndoView, 0);
    bottomLayout->addWidget(&mTextureWidget, 1);

    // Bottom Splitter
    mBottomSplitter = new QSplitter(Qt::Vertical, this);
    mBottomSplitter->addWidget(mTopSplitter);
    mBottomSplitter->addWidget(bottomContainer);
    mBottomSplitter->setStretchFactor(0, 1);
    mBottomSplitter->setStretchFactor(1, 0);

    // Ptcl List
    mPtclList.setEnabled(false);
    mPtclList.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    mPtclList.setSelection(&mSelection);

    // Inspector
    mInspector.setEnabled(false);
    mInspector.setSelection(&mSelection);

    // Texture Widget
    mTextureWidget.setEnabled(false);

    setCentralWidget(mBottomSplitter);
    setupMenus();

    // Undo View
    mUndoView.setEmptyLabel("<No History>");

    // StatusBar
    auto* status = statusBar();
    mStatusLabel = new QLabel("No file loaded", status);
    status->addPermanentWidget(mStatusLabel);

    auto& settings = SettingsUtil::SettingsMgr::instance();
    restoreGeometry(settings.windowGeometry());
    restoreState(settings.windowState());

    updateWindowTitle();
}

void MainWindow::setupMenus() {
    // Open File
    mOpenAction.setText("Open File");
    mOpenAction.setIcon(QIcon(":/res/icons/open_file.png"));
    mOpenAction.setShortcut(QKeySequence::Open);
    connect(&mOpenAction, &QAction::triggered, this, &MainWindow::openFile);

    // Save
    mSaveAction.setText("Save");
    mSaveAction.setIcon(QIcon(":/res/icons/save.png"));
    mSaveAction.setShortcut(QKeySequence::Save);
    mSaveAction.setEnabled(false);
    connect(&mSaveAction, &QAction::triggered, this, &MainWindow::saveFile);

    // Save As
    mSaveAsAction.setText("Save As");
    mSaveAsAction.setIcon(QIcon(":/res/icons/save_as.png"));
    mSaveAsAction.setShortcut(QKeySequence::SaveAs);
    mSaveAsAction.setEnabled(false);
    connect(&mSaveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);

    // Undo
    mUndoAction = new QAction("Undo", this);
    mUndoAction->setShortcut(QKeySequence::Undo);
    mUndoAction->setEnabled(false);

    // Redo
    mRedoAction = new QAction("Redo", this);
    mRedoAction->setShortcut(QKeySequence::Redo);
    mRedoAction->setEnabled(false);

    // Recent Files Menu
    mRecentFilesMenu.setTitle("Recent Files");
    mRecentFilesMenu.setIcon(QIcon(":/res/icons/recent.png"));

    // Recent Files Actions
    s32 maxRecentFiles = SettingsUtil::SettingsMgr::instance().maxRecentFiles();
    for (s32 i = 0; i < maxRecentFiles; ++i) {
        QAction* recentFileAction = mRecentFilesMenu.addAction("");
        recentFileAction->setVisible(false);
        connect(recentFileAction, &QAction::triggered, this, &MainWindow::openRecentFile);
        mRecentFileActions.push_back(recentFileAction);
    }

    // File Menu
    mFileMenu.setTitle("File");
    mFileMenu.addAction(&mOpenAction);
    mFileMenu.addAction(&mSaveAction);
    mFileMenu.addAction(&mSaveAsAction);
    mFileMenu.addSeparator();
    mFileMenu.addMenu(&mRecentFilesMenu);

    // Edit Menu
    mEditMenu.setTitle("Edit");
    mEditMenu.addAction(mUndoAction);
    mEditMenu.addAction(mRedoAction);

    // Menu Bar
    menuBar()->addMenu(&mFileMenu);
    menuBar()->addMenu(&mEditMenu);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (!mDocument || !mDocument->isDirty()) {
        auto& settings = SettingsUtil::SettingsMgr::instance();
        settings.setWindowGeometry(saveGeometry());
        settings.setWindowState(saveState());

        event->accept();
        return;
    }

    QMessageBox::StandardButton result = QMessageBox::warning(this,
        "Unsaved Changes",
        "The current file has unsaved changes.\n\nDo you want to save them before closing?",
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel,
        QMessageBox::Save
    );

    switch (result) {
    case QMessageBox::Save:
        saveFile();
        break;
    case QMessageBox::Discard:
        break;
    case QMessageBox::Cancel:
    default:
        event->ignore();
        return;
    }

    auto& settings = SettingsUtil::SettingsMgr::instance();
    settings.setWindowGeometry(saveGeometry());
    settings.setWindowState(saveState());

    event->accept();
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
    if (!mDocument) {
        return;
    }

    if (mDocument->filePath().isEmpty()) {
        saveFileAs();
        return;
    }

    mDocument->save(mDocument->filePath());

    statusBar()->showMessage("File Saved", 2000);

    SettingsUtil::SettingsMgr::instance().addRecentFile(mDocument->filePath());
    updateRecentFileList();
}

void MainWindow::saveFileAs() {
    if (!mDocument) {
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

    QFileDialog dialog(this, "Save As", basePath, "*.ptcl");
    if(dialog.exec() == QFileDialog::DialogCode::Rejected) {
        return;
    }

    auto filePath = dialog.selectedFiles().constFirst();
    if (!filePath.endsWith(".ptcl")) {
        filePath += ".ptcl";
    }

    mDocument->save(filePath);

    statusBar()->showMessage("File Saved", 2000);

    mDocument->filePath() = filePath;
    SettingsUtil::SettingsMgr::instance().addRecentFile(filePath);
    SettingsUtil::SettingsMgr::instance().setLastSavePath(QFileInfo(filePath).absolutePath());
    updateRecentFileList();
    updateWindowTitle();
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

    s32 maxRecentFiles = SettingsUtil::SettingsMgr::instance().maxRecentFiles();
    qsizetype numRecentFiles = qMin(recentFiles.size(), maxRecentFiles);

    for (s32 i = 0; i < numRecentFiles; ++i) {
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
    mPtclList.setDocument(nullptr);
    mInspector.setDocument(nullptr);
    mTextureWidget.setDocument(nullptr);

    mSaveAsAction.setEnabled(false);

    mDocument = std::make_unique<Ptcl::Document>();
    if (!mDocument->load(path)) {
        mDocument.reset();
        bindUndoStack();
        return;
    }

    bindUndoStack();

    SettingsUtil::SettingsMgr::instance().addRecentFile(path);
    SettingsUtil::SettingsMgr::instance().setLastOpenPath(QFileInfo(path).absolutePath());
    updateRecentFileList();

    mPtclList.setDocument(mDocument.get());
    mInspector.setDocument(mDocument.get());
    mTextureWidget.setDocument(mDocument.get());

    if (mDocument->emitterSetCount() != 0) {
        mSelection.set(0, 0, Ptcl::Selection::Type::EmitterSet);
    }

    updateStatusBar();
    updateWindowTitle();

    mSaveAsAction.setEnabled(true);
}

void MainWindow::updateWindowTitle() {
    QString title = "PTCLTool";
    if (mDocument && !mDocument->filePath().isEmpty()) {
        title += " - " + QFileInfo(mDocument->filePath()).fileName();
    }
    if (mDocument && mDocument->isDirty()) {
        title += " *";
    }
    setWindowTitle(title);
}

void MainWindow::updateStatusBar() {
    if (!mStatusLabel) {
        return;
    }

    if (!mDocument) {
        mStatusLabel->setText("No file loaded");
    } else if (mDocument->isDirty()) {
        mStatusLabel->setText("Unsaved changes");
    } else {
        mStatusLabel->setText("All changes saved");
    }
}

void MainWindow::bindUndoStack() {
    if (!mDocument) {
        mUndoAction->setEnabled(false);
        mRedoAction->setEnabled(false);
        return;
    }

    auto* stack = mDocument->undoStack();

    mUndoAction->disconnect();
    mRedoAction->disconnect();

    connect(mUndoAction, &QAction::triggered, stack, &QUndoStack::undo);
    connect(mRedoAction, &QAction::triggered, stack, &QUndoStack::redo);

    connect(stack, &QUndoStack::canUndoChanged, mUndoAction, &QAction::setEnabled);
    connect(stack, &QUndoStack::canRedoChanged, mRedoAction, &QAction::setEnabled);

    connect(stack, &QUndoStack::undoTextChanged, this, [this](const QString& text) {
        mUndoAction->setText(text.isEmpty() ? "Undo" : "Undo " + text);
    });

    connect(stack, &QUndoStack::redoTextChanged, this, [this](const QString& text) {
        mRedoAction->setText(text.isEmpty() ? "Redo" : "Redo " + text);
    });

    connect(stack, &QUndoStack::cleanChanged, this, [this](bool clean) {
        const bool dirty = !clean;
        mSaveAction.setEnabled(dirty);
        updateStatusBar();
        updateWindowTitle();
    });

    mUndoAction->setEnabled(stack->canUndo());
    mRedoAction->setEnabled(stack->canRedo());

    mUndoView.setStack(stack);
}


// ========================================================================== //


} // namespace PtclEditor
