#pragma once

#include <QDataStream>
#include <QFile>
#include <QImageReader>


namespace FileUtil {


// ========================================================================== //


enum class FileType {
    PtclBinary,
    Image,
    Unknown
};


// ========================================================================== //


bool isPtclBinary(const QString& path) {
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QByteArray magic = file.read(4);
    return magic == "SPBD";
}

FileType classifyFile(const QString& path) {
    if (QImageReader(path).canRead()) {
        return FileType::Image;
    }

    if (isPtclBinary(path)) {
        return FileType::PtclBinary;
    }

    return FileType::Unknown;
}


// ========================================================================== //


} // namespace FileUtil
