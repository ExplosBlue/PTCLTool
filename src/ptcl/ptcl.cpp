#include "ptcl/ptcl.h"

#include <QDataStream>
#include <QFile>

#include "ptcl/ptclRes.h"
#include "imageutil.h"

namespace Ptcl {

void PtclRes::load(const QString& filePath) {

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        throw("PtclRes::load - Failed to open file.");
        return;
    }

    file.seek(0);

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    // Read Binary Header
    BinHeaderData binHeader;
    stream >> binHeader;

    // Read tables
    s64 emitterSetTblPos = file.pos();

    file.seek(binHeader.nameTblPos);
    QByteArray nameTbl = file.readAll();

    file.seek(binHeader.textureTblPos);
    s64 textureTblPos = file.pos();

    // Read EmitterSets
    mEmitterSets.reserve(binHeader.numEmitterSet);

    for (int resEmitterSetIdx = 0; resEmitterSetIdx < binHeader.numEmitterSet; resEmitterSetIdx++) {

        EmitterSet emitterSet;

        file.seek(emitterSetTblPos + (sizeof(BinEmitterSetData) * resEmitterSetIdx));
        BinEmitterSetData binEmitterSetData;
        stream >> binEmitterSetData;

        const char* name = nameTbl.data() + binEmitterSetData.namePos;
        emitterSet.setName(name);
        emitterSet.setUserData(binEmitterSetData.userData);

        emitterSet.emitters().reserve(binEmitterSetData.numEmitter);

        // Read Emitters
        if (binEmitterSetData.numEmitter > 0) {

            int emitterIdx = 0;
            while (binEmitterSetData.numEmitter > emitterIdx) {

                Emitter emitter;

                file.seek(binEmitterSetData.emitterTblPos + (sizeof(binEmitterSetData) * emitterIdx));
                BinEmitterTblData binEmitterTblData;
                stream >> binEmitterTblData;

                if (binEmitterTblData.emitterPos > 0) {

                    file.seek(binEmitterTblData.emitterPos);
                    BinCommonEmitterData binCommonEmitterData;
                    stream >> binCommonEmitterData;

                    const char* name = nameTbl.data() + binCommonEmitterData.namePos;
                    emitter.setName(name);

                    file.seek(textureTblPos + binCommonEmitterData.texturePos);
                    QByteArray textureData;

                    qDebug() << "TextureSize: " << binCommonEmitterData.textureSize;


                    auto height = binCommonEmitterData.textureRes.height;
                    auto width = binCommonEmitterData.textureRes.width;
                    auto format = binCommonEmitterData.textureRes.format;
                    qDebug() << textureData;

                    // Unsure if alignment here is needed...
                    textureData.resize(ImageUtil::getTextureSize(width, height, format, 32));
                    stream.readRawData(textureData.data(), ImageUtil::getTextureSize(width, height, format, 32));

                    emitter.setTexture(ImageUtil::picaTextureToQImage(textureData, width, height, format));
                    emitter.setTextureFormat(format);
                }

                emitterSet.emitters().push_back(emitter);

                ++emitterIdx;
            }
        }
        mEmitterSets.push_back(emitterSet);
    }
}

} // namespace Ptcl
