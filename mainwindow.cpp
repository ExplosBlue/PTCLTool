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

    QByteArray* ptclResData = new QByteArray(file.readAll());
    file.seek(0);

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
//        qDebug() << basePos + ptcl::RES_HEADER_DATA_SIZE + (ptcl::EMITTER_SET_DATA_SIZE * resEmiterSetIdx);

        file.seek(basePos + ptcl::RES_HEADER_DATA_SIZE + (ptcl::EMITTER_SET_DATA_SIZE * resEmiterSetIdx));
        ptcl::EmitterSetData* emitterSetData = new ptcl::EmitterSetData();
        stream >> *emitterSetData;

//        emitterSetData->printData();
//        break;

        emitterSetData->name = nameTbl.data() + emitterSetData->namePos;

        resEmitterSet.setName      = emitterSetData->name;
        resEmitterSet.setData      = emitterSetData;
        resEmitterSet.numEmitter   = emitterSetData->numEmitter;
        resEmitterSet.userData     = emitterSetData->userData;
        resEmitterSet.isShowDetail = false;

        if (emitterSetData->emitterTblPos) {

            // TODO: Should be an array of all emitter tables
            file.seek(basePos + emitterSetData->emitterTblPos);
            ptcl::EmitterTblData tblData;
            stream >> tblData;

            resEmitterSet.tblData;

            if (resEmitterSet.numEmitter > 0) {

                int emitterIdx = 0;
                while (resEmitterSet.numEmitter > emitterIdx) {

                    file.seek(basePos + emitterSetData->emitterTblPos + ptcl::EMITTER_TBL_DATA_SIZE * emitterIdx);
                    ptcl::EmitterTblData emitterTblData;
                    stream >> emitterTblData;

                    if (emitterTblData.emitterPos) {

                        file.seek(basePos + emitterTblData.emitterPos);
                        ptcl::CommonEmitterData* commonEmitterData = new ptcl::CommonEmitterData();
                        stream >> *commonEmitterData;

                        emitterTblData.emitter = commonEmitterData;

                        commonEmitterData->name  = nameTbl.data() + commonEmitterData->namePos;

                        // emitterTblData->emitter->texture = someFunc();
                        if (emitterTblData.emitter->type == ptcl::EFT_EMITTER_TYPE_COMPLEX ) {
                           // emitterTblData->emitter->texture = someFunc();
                        }

                        resEmitterSet.tblData.append(emitterTblData);

                    } else {

//                        emitterTblData = nullptr;
                    }

                    ++emitterIdx;
                }

            }

        } else {

//            resEmitterSet.tblData = nullptr;
        }

        resEmitterSet.userDataROM   = resEmitterSet.userData;
        resEmitterSet.setDataROM    = resEmitterSet.setData;
        resEmitterSet.tblDataROM    = resEmitterSet.tblData;
        resEmitterSet.numEmitterROM = resEmitterSet.numEmitter;

        mEmitterSets.append(resEmitterSet);
    }

    for (int i = 0; i < mEmitterSets.length(); i++) {
        qDebug() << "ResourceEmitterSet" << i << ":";
        ptcl::ResourceEmitterSet set = mEmitterSets.at(i);
        set.printData();
    }

}
