#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Resource.h"

#include <QFileDialog>
#include <QFile>
#include <QDataStream>

QString readString(QDataStream& stream) {

    // TODO: prevent infinite loop if string never terminates

    QString buffer;
    bool terminated = false;

    while (!terminated) {

        char temp;
        stream >> temp;

        if (temp == 0) {
            terminated = true;
            break;
        }

        buffer.append(temp);
    }

    return buffer;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    LoadData();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::LoadData() {

    QString filename = QFileDialog::getOpenFileName(this, "Select File", "D://Explos//Home//Modding//3DS//Games//NSMB2//Rom//Gold Edition - USA//ExtractedRomFS//Effect");

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

//    QByteArray ptclResData = file.readAll();

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    quint64 basePos = file.pos();

    // Header
    ptcl::HeaderData mHeader;

    stream >> mHeader;
    mHeader.printData();

    quint64 resEmitterSetTblPos = file.pos();

    file.seek(basePos + mHeader.nameTblPos);
    QByteArray nameTbl = file.readAll();

    file.seek(basePos + mHeader.textureTblPos);
    quint64 textureTblPos = file.pos();

    // ResourceEmitterSets
    QList<ptcl::ResourceEmitterSet> mEmitterSets;

    for (int resEmiterSetIdx = 0; resEmiterSetIdx < mHeader.numEmitterSet; resEmiterSetIdx++) {

        ptcl::ResourceEmitterSet resEmitterSet;

        // EmitterSetData
        file.seek(basePos + ptcl::EMITTER_SET_DATA_SIZE * resEmiterSetIdx);
        ptcl::EmitterSetData* emitterSetData = new ptcl::EmitterSetData();
        stream >> *emitterSetData;

        emitterSetData->name = nameTbl.data() + emitterSetData->namePos;

        resEmitterSet.setName      = emitterSetData->name;
        resEmitterSet.setData      = emitterSetData;
        resEmitterSet.numEmitter   = emitterSetData->numEmitter;
        resEmitterSet.userData     = emitterSetData->userData;
        resEmitterSet.isShowDetail = false;

        if (emitterSetData->emitterTblPos) {

            // TODO

        } else {

            resEmitterSet.tblData = nullptr;
        }

        resEmitterSet.userDataROM   = resEmitterSet.userData;
        resEmitterSet.setDataROM    = resEmitterSet.setData;
        resEmitterSet.tblDataROM    = resEmitterSet.tblData;
        resEmitterSet.numEmitterROM = resEmitterSet.numEmitter;

        mEmitterSets.append(resEmitterSet);
    }

    foreach(ptcl::ResourceEmitterSet set, mEmitterSets) {
        set.printData();
    }

}
