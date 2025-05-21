#include "mainwindow.h"
#include "../ui/ui_mainwindow.h"

#include "ptcl/ptcl.h"

#include <QFileDialog>
#include <QFile>
#include <QDataStream>
#include <QHBoxLayout>

// TODO: REMOVE THIS
static const QString basePath = "D://Explos//Home//Modding//3DS//Games//NSMB2//Rom//Gold Edition - USA//ExtractedRomFS//Effect";

QString readString(QDataStream& stream) {

    // TODO: prevent infinite loop if string never terminates

    QString buffer;

    while (true) {

        char temp;
        stream >> temp;

        if (temp == 0) {
            break;
        }

        buffer.append(temp);
    }

    return buffer;
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    LoadData();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::LoadData() {

    QString filename = QFileDialog::getOpenFileName(this, "Select File", basePath);

    mPtclRes = std::make_unique<Ptcl::PtclRes>();
    mPtclRes->load(filename);

    mPtclRes->getEmitterSets();

    mPtclList = std::make_unique<PtclList>(mPtclRes.get(), this);
    connect(&mEmitterPicker, &QComboBox::currentIndexChanged, this, &MainWindow::selectedEmitterChanged);
    connect(mPtclList.get(), &PtclList::selectedIndexChanged, this, &MainWindow::selectedEmitterSetChanged);

    mPtclList->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    mCenteralWidget.setLayout(&mMainLayout);

    mMainLayout.addWidget(mPtclList.get());
    mMainLayout.addLayout(&mImageInfoLayout);

    mImageInfoLayout.addWidget(&mEmitterPicker);
    mImageInfoLayout.addWidget(&mImageLabel);
    mImageInfoLayout.addWidget(&mImageInfoLabel);

    setCentralWidget(&mCenteralWidget);
}

void MainWindow::selectedEmitterChanged(u32 index) {

    auto &emitterSet = mPtclRes->getEmitterSets()[mCurEmitterSetIdx];

    // Check if the emitter set exists and has emitters
    if (mCurEmitterSetIdx < mPtclRes->getEmitterSets().size() && index < emitterSet.emitters().size()) {
        QImage texture = emitterSet.emitters()[index].texture();

        auto format = emitterSet.emitters()[index].textureFormat();
        auto width = texture.width();
        auto height = texture.height();

        QPixmap pixmap = QPixmap::fromImage(texture);
        mImageLabel.setPixmap(pixmap.scaled(width * 3, height * 3, Qt::KeepAspectRatio));

        QString labelText = QString("Format: %1 \n Width: %2 \n Height: %3").arg(gr::toQString(format)).arg(width).arg(height);
        mImageInfoLabel.setText(labelText);
    }
}

void MainWindow::selectedEmitterSetChanged(u32 index) {

    mCurEmitterSetIdx = index;
    auto &emitterSet = mPtclRes->getEmitterSets()[index];

    mEmitterPicker.blockSignals(true);

    mEmitterPicker.clear();

    u32 idx = 0;
    for (auto& emitter : emitterSet.emitters()) {
        mEmitterPicker.addItem(emitter.name(), idx);
        ++idx;
    }

    mEmitterPicker.blockSignals(false);

    selectedEmitterChanged(0);
}
