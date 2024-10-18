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
    mEmitterSetWidget = std::make_unique<EmitterSetWidget>(this);
    mCenteralWidget = new QWidget(this);

    connect(mPtclList.get(), &PtclList::selectedIndexChanged, this, &MainWindow::selectedEmitterSetChanged);

    mPtclList->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    mCenteralWidget->setLayout(&mMainLayout);

    mMainLayout.addWidget(mPtclList.get());
    mMainLayout.addWidget(mEmitterSetWidget.get());

    setCentralWidget(mCenteralWidget);
}

void MainWindow::selectedEmitterSetChanged(u32 index) {

    mCurEmitterSetIdx = index;
    auto &emitterSet = mPtclRes->getEmitterSets()[index];

    mEmitterSetWidget->setEmitterSet(&emitterSet);

    // selectedEmitterChanged(0);
}


