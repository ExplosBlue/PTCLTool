#include "editor/mainWindow.h"
#include "util/nameValidator.h"
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
    setupConnections();
    updateRecentFileList();
}

void MainWindow::setupUi() {
    // MainWindow
    setAcceptDrops(true);

    // Properties
    mPropertiesStack = new QStackedWidget(this);
    mPropertiesStack->addWidget(&mEmitterSetWidget);
    mPropertiesStack->addWidget(&mEmitterWidget);
    mPropertiesStack->setCurrentIndex(0);

    mPropertiesGroup.setFlat(false);
    auto* emitterGroupLayout = new QVBoxLayout(&mPropertiesGroup);
    emitterGroupLayout->setContentsMargins(0, 0, 0, 0);
    emitterGroupLayout->addWidget(mPropertiesStack);

    // Project Name
    mProjNameLineEdit.setPlaceholderText("PTCLProject");
    mProjNameLineEdit.setValidator(new EmitterNameValidator(&mProjNameLineEdit));
    mProjNameLineEdit.setEnabled(false);

    auto* projectSettingsLayout = new QFormLayout;
    projectSettingsLayout->setContentsMargins(0, 0, 0, 0);
    projectSettingsLayout->addRow("Project Name:", &mProjNameLineEdit);

    auto* propertiesContainer = new QWidget(this);
    auto* propertiesLayout = new QVBoxLayout(propertiesContainer);
    propertiesLayout->setContentsMargins(0, 0, 0, 0);
    propertiesLayout->setSpacing(4);

    propertiesLayout->addLayout(projectSettingsLayout);
    propertiesLayout->addWidget(&mPropertiesGroup);

    // Top Splitter
    mTopSplitter = new QSplitter(Qt::Horizontal, this);
    mTopSplitter->addWidget(&mPtclList);
    mTopSplitter->addWidget(propertiesContainer);
    mTopSplitter->setSizes({1, 9999});

    // Bottom Splitter
    mBottomSplitter = new QSplitter(Qt::Vertical, this);
    mBottomSplitter->addWidget(mTopSplitter);
    mBottomSplitter->addWidget(&mTextureWidget);

    // Ptcl List
    mPtclList.setEnabled(false);
    mPtclList.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    // Emitter Widget
    mEmitterWidget.setEnabled(false);
    mEmitterWidget.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // EmitterSet Widget
    mEmitterSetWidget.setEnabled(false);
    mEmitterSetWidget.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Texture Widget
    mTextureWidget.setEnabled(false);

    setCentralWidget(mBottomSplitter);
    setupMenus();

    // StatusBar
    auto* status = statusBar();
    mStatusLabel = new QLabel("No file loaded", status);
    status->addPermanentWidget(mStatusLabel);

    auto& settings = SettingsUtil::SettingsMgr::instance();
    restoreGeometry(settings.windowGeometry());
    restoreState(settings.windowState());

    updateWindowTitle();
}

void MainWindow::setupConnections() {
    // Proj Name
    connect(&mProjNameLineEdit, &QLineEdit::textChanged, this, [this](const QString& text) {
        mPtclRes->setName(text);
        setDirty(true);
    });

    // Ptcl List
    connect(&mPtclList, &PtclList::selectedEmitterSetChanged, this, [this](s32 index) {
        selectEmitterSet(index);
        setPropertiesView(PropertiesView::EmitterSet);
        updatePropertiesStatus();
    });

    connect(&mPtclList, &PtclList::selectedEmitterChanged, this, [this](s32 setIndex, s32 emitterIndex) {
        selectEmitter(setIndex, emitterIndex);
        setPropertiesView(PropertiesView::Emitter);
        updatePropertiesStatus();
    });

    connect(&mPtclList, &PtclList::selectedChildData, this, [this](s32 setIndex, s32 emitterIndex) {
        selectEmitter(setIndex, emitterIndex);
        setPropertiesView(PropertiesView::EmitterChild);
        updatePropertiesStatus();
    });

    connect(&mPtclList, &PtclList::selectedFluctuation, this, [this](s32 setIndex, s32 emitterIndex) {
        selectEmitter(setIndex, emitterIndex);
        setPropertiesView(PropertiesView::EmitterFlux);
        updatePropertiesStatus();
    });

    connect(&mPtclList, &PtclList::selectedField, this, [this](s32 setIndex, s32 emitterIndex) {
        selectEmitter(setIndex, emitterIndex);
        setPropertiesView(PropertiesView::EmitterField);
        updatePropertiesStatus();
    });

    connect(&mPtclList, &PtclList::itemAdded, this, [this]() {
        setDirty(true);
    });

    connect(&mPtclList, &PtclList::itemRemoved, this, [this]() {
        setDirty(true);
    });

    // Emitter Widget
    connect(&mEmitterWidget, &EmitterWidget::textureUpdated, this, [this](s32 oldIndex, s32 newIndex) {
        if (oldIndex >= 0) { mTextureWidget.updateItemAt(oldIndex); }
        if (newIndex >= 0) { mTextureWidget.updateItemAt(newIndex); }
    });

    connect(&mEmitterWidget, &EmitterWidget::emitterNameChanged, this, [this]() {
        mPtclList.updateEmitterName(mCurEmitterSetIdx, mCurEmitterIdx);
        updatePropertiesStatus();
    });

    connect(&mEmitterWidget, &EmitterWidget::emitterTypeChanged, this, [this]() {
        mPtclList.updateEmitter(mCurEmitterSetIdx, mCurEmitterIdx);
    });

    connect(&mEmitterWidget, &EmitterWidget::complexFlagsChanged, this, [this]() {
        mPtclList.updateEmitter(mCurEmitterSetIdx, mCurEmitterIdx);
    });

    connect(&mEmitterWidget, &EmitterWidget::propertiesChanged, this, [this]() {
        setDirty(true);
    });

    // EmitterSet Widget
    connect(&mEmitterSetWidget, &EmitterSetWidget::emitterSetNamedChanged, this, [this]() {
        mPtclList.updateEmitterSetName(mCurEmitterSetIdx);
        updatePropertiesStatus();
    });

    connect(&mEmitterSetWidget, &EmitterSetWidget::propertiesChanged, this, [this]() {
        setDirty(true);
    });
}

void MainWindow::setupMenus() {
    // Open File
    mOpenAction.setText("Open File");
    mOpenAction.setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen));
    mOpenAction.setShortcut(QKeySequence::Open);
    connect(&mOpenAction, &QAction::triggered, this, &MainWindow::openFile);

    // Save
    mSaveAction.setText("Save");
    mSaveAction.setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSave));
    mSaveAction.setShortcut(QKeySequence::Save);
    mSaveAction.setEnabled(false);
    connect(&mSaveAction, &QAction::triggered, this, &MainWindow::saveFile);

    // Save As
    mSaveAsAction.setText("Save As");
    mSaveAsAction.setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSaveAs));
    mSaveAsAction.setShortcut(QKeySequence::SaveAs);
    mSaveAsAction.setEnabled(false);
    connect(&mSaveAsAction, &QAction::triggered, this, &MainWindow::saveFileAs);

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
    mFileMenu.addAction(&mSaveAsAction);
    mFileMenu.addSeparator();
    mFileMenu.addMenu(&mRecentFilesMenu);

    // Menu Bar
    menuBar()->addMenu(&mFileMenu);
}

void MainWindow::closeEvent(QCloseEvent* event) {
    if (!mPtclRes || !mHasUnsavedChanges) {
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
    if (!mPtclRes) {
        return;
    }

    if (mCurrentFilePath.isEmpty()) {
        saveFileAs();
        return;
    }

    mPtclRes->save(mCurrentFilePath);

    setDirty(false);
    statusBar()->showMessage("File Saved", 2000);

    SettingsUtil::SettingsMgr::instance().addRecentFile(mCurrentFilePath);
    updateRecentFileList();
}

void MainWindow::saveFileAs() {
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

    QFileDialog dialog(this, "Save As", basePath, "*.ptcl");
    if(dialog.exec() == QFileDialog::DialogCode::Rejected) {
        return;
    }

    auto filePath = dialog.selectedFiles().constFirst();
    if (!filePath.endsWith(".ptcl")) {
        filePath += ".ptcl";
    }

    mPtclRes->save(filePath);

    setDirty(false);
    statusBar()->showMessage("File Saved", 2000);

    mCurrentFilePath = filePath;
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
    mProjNameLineEdit.setEnabled(false);
    mSaveAsAction.setEnabled(false);

    mPtclRes = std::make_unique<Ptcl::PtclRes>();
    if (!mPtclRes->load(path)) {
        mPtclRes.reset();
        return;
    }

    mCurrentFilePath = path;
    setDirty(false);

    SettingsUtil::SettingsMgr::instance().addRecentFile(path);
    SettingsUtil::SettingsMgr::instance().setLastOpenPath(QFileInfo(path).absolutePath());
    updateRecentFileList();

    mPtclList.setPtclRes(mPtclRes.get());
    mTextureWidget.setTextures(&mPtclRes->textures());
    mEmitterWidget.setTextureList(&mPtclRes->textures());

    QSignalBlocker b1(mProjNameLineEdit);
    mProjNameLineEdit.setText(mPtclRes->name());

    const auto& sets = mPtclRes->getEmitterSets();
    if (!sets.empty()) {
        selectEmitter(0, 0);
    }

    updateStatusBar();
    updateWindowTitle();

    mPtclList.setEnabled(true);
    mTextureWidget.setEnabled(true);
    mProjNameLineEdit.setEnabled(true);
    mSaveAsAction.setEnabled(true);
}

void MainWindow::selectEmitterSet(s32 setIndex) {
    mCurEmitterSetIdx = setIndex;
    mCurEmitterIdx = 0;

    if (!mPtclRes || mCurEmitterIdx < 0 || mCurEmitterIdx < 0) {
        return;
    }

    const auto& emitterSets = mPtclRes->getEmitterSets();
    if (mCurEmitterSetIdx >= emitterSets.size()) {
        return;
    }
    const auto& emitterSet = emitterSets[mCurEmitterSetIdx];

    mEmitterSetWidget.setEmitterSet(emitterSet.get());

    if (!mEmitterSetWidget.isEnabled()) {
        mEmitterSetWidget.setEnabled(true);
    }
}

void MainWindow::selectEmitter(s32 setIndex, s32 emitterIndex) {
    mCurEmitterSetIdx = setIndex;
    mCurEmitterIdx = emitterIndex;

    if (!mPtclRes || mCurEmitterIdx < 0 || mCurEmitterIdx < 0) {
        return;
    }

    const auto& emitterSets = mPtclRes->getEmitterSets();
    if (mCurEmitterSetIdx >= emitterSets.size()) {
        return;
    }
    const auto& emitterSet = emitterSets[mCurEmitterSetIdx];

    const auto& emitters = emitterSet->emitters();
    if (mCurEmitterIdx >= emitters.size()) {
        return;
    }
    const auto& emitter = emitters[mCurEmitterIdx];

    mEmitterWidget.setEmitter(emitter.get());

    if (!mEmitterWidget.isEnabled()) {
        mEmitterWidget.setEnabled(true);
    }
}

void MainWindow::setPropertiesView(PropertiesView view) {
    if (view == mCurPropertiesView) {
        return;
    }

    mCurPropertiesView = view;

    switch (mCurPropertiesView) {
        case PropertiesView::EmitterSet:
            mPropertiesStack->setCurrentWidget(&mEmitterSetWidget);
            break;
        case PropertiesView::Emitter:
            mPropertiesStack->setCurrentWidget(&mEmitterWidget);
            mEmitterWidget.showStandardEditor();
            break;
        case PropertiesView::EmitterChild:
            mPropertiesStack->setCurrentWidget(&mEmitterWidget);
            mEmitterWidget.showChildEditor();
            break;
        case PropertiesView::EmitterFlux:
            mPropertiesStack->setCurrentWidget(&mEmitterWidget);
            mEmitterWidget.showFluctuationEditor();
            break;
        case PropertiesView::EmitterField:
            mPropertiesStack->setCurrentWidget(&mEmitterWidget);
            mEmitterWidget.showFieldEditor();
            break;
    }
}

void MainWindow::updatePropertiesStatus() {
    mPropertiesGroup.setTitle({});

    if (!mPtclRes || mCurEmitterIdx < 0 || mCurEmitterIdx < 0) {
        return;
    }

    const auto& emitterSets = mPtclRes->getEmitterSets();
    if (mCurEmitterSetIdx >= emitterSets.size()) {
        return;
    }
    const auto& emitterSet = emitterSets[mCurEmitterSetIdx];

    const auto& emitters = emitterSet->emitters();
    if (mCurEmitterIdx >= emitters.size()) {
        return;
    }
    const auto& emitter = emitters[mCurEmitterIdx];

    QString title = emitterSet->name();

    if (mCurPropertiesView != PropertiesView::EmitterSet) {
        title += QStringLiteral(" > %1").arg(emitter->name());
    }

    switch (mCurPropertiesView) {
    case PropertiesView::EmitterChild:
        title += QStringLiteral(" > Child Properties:");
        break;
    case PropertiesView::EmitterFlux:
        title += QStringLiteral(" > Fluctuation Properties:");
        break;
    case PropertiesView::EmitterField:
        title += QStringLiteral(" > Field Properties:");
        break;
    case PropertiesView::Emitter:
    case PropertiesView::EmitterSet:
        title += QStringLiteral(" Properties:");
        break;
    }

    mPropertiesGroup.setTitle(title);
}

void MainWindow::updateWindowTitle() {
    QString title = "PtclEditor";
    if (!mCurrentFilePath.isEmpty()) {
        title += " - " + QFileInfo(mCurrentFilePath).fileName();
    }
    if (mHasUnsavedChanges) {
        title += " *";
    }
    setWindowTitle(title);
}

void MainWindow::updateStatusBar() {
    if (!mStatusLabel) {
        return;
    }

    if (!mPtclRes) {
        mStatusLabel->setText("No file loaded");
    } else if (mHasUnsavedChanges) {
        mStatusLabel->setText("Unsaved changes");
    } else {
        mStatusLabel->setText("All changes saved");
    }
}

void MainWindow::setDirty(bool dirty) {
    mHasUnsavedChanges = dirty;
    mSaveAction.setEnabled(dirty);
    updateStatusBar();
    updateWindowTitle();
}


// ========================================================================== //


} // namespace PtclEditor
