#pragma once

#include "typedefs.h"

#include <iconv.h>

#include <QDir>
#include <QFontMetrics>
#include <QString>

#include <stdexcept>


namespace StringUtil {


// ========================================================================== //


inline bool iconv_failed(iconv_t cd) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-reinterpret-cast)
    return reinterpret_cast<std::intptr_t>(cd) == -1;
}

inline QString shiftJISToQString(const char* input, size_t inputLength = 0) {
    if (!input) {
        return {};
    }

    if (inputLength == 0) {
        inputLength = std::strlen(input);
    }

    iconv_t cd = iconv_open("UTF-8", "SHIFT-JIS");

    if (iconv_failed(cd)) {
        throw std::runtime_error("iconv_open failed");
    }

    size_t inBytesLeft = inputLength;
    size_t outBytesLeft = inputLength * 4;

    if (outBytesLeft > static_cast<size_t>(std::numeric_limits<qsizetype>::max())) {
        throw std::runtime_error("Output buffer size too large");
    }

    QByteArray outputBuffer(static_cast<qsizetype>(outBytesLeft), 0);
    char* outBuf = outputBuffer.data();

#ifdef Q_OS_WIN
    const char* inBufPtr = input;
    const char** inBuf = &inBufPtr;
    size_t result = iconv(cd,
                          inBuf,
                          &inBytesLeft,
                          &outBuf,
                          &outBytesLeft);
#else
    char* inBuf = const_cast<char*>(input);
    size_t result = iconv(cd,
                          &inBuf,
                          &inBytesLeft,
                          &outBuf,
                          &outBytesLeft);
#endif

    iconv_close(cd);

    if (result == size_t(-1)) {
        throw std::runtime_error("iconv conversion failed");
    }

    size_t byteCount = outputBuffer.size() - outBytesLeft;
    if (byteCount > static_cast<size_t>(std::numeric_limits<qsizetype>::max())) {
        throw std::runtime_error("Converted string size too large");
    }

    auto length = static_cast<qsizetype>(byteCount);
    return QString::fromUtf8(outputBuffer.constData(), length);
}

inline QByteArray qStringToShiftJIS(const QString& input) {
    if (input.isEmpty()) {
        return {};
    }

    iconv_t cd = iconv_open("SHIFT-JIS", "UTF-8");
    if (iconv_failed(cd)) {
        throw std::runtime_error("iconv_open failed");
    }

    QByteArray utf8 = input.toUtf8();
    const char* inBufPtr = utf8.constData();
    size_t inBytesLeft = utf8.size();

    size_t outBytesLeft = inBytesLeft * 4;
    if (outBytesLeft > static_cast<size_t>(std::numeric_limits<qsizetype>::max())) {
        iconv_close(cd);
        throw std::runtime_error("Output buffer size too large");
    }

    QByteArray outputBuffer(static_cast<qsizetype>(outBytesLeft), 0);
    char* outBuf = outputBuffer.data();

#ifdef Q_OS_WIN
    const char** inBuf = &inBufPtr;
    size_t result = iconv(cd, inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
#else
    char* inBuf = const_cast<char*>(inBufPtr);
    size_t result = iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
#endif

    iconv_close(cd);

    if (result == size_t(-1)) {
        throw std::runtime_error("iconv conversion failed");
    }

    auto writtenSize = static_cast<qsizetype>(outputBuffer.size() - outBytesLeft);
    return outputBuffer.left(writtenSize);
}

inline QString elidePath(const QString& path, const QFontMetrics& metrics, s32 width)
{
    QString nativePath = QDir::toNativeSeparators(path);
    const QString homePath = QDir::toNativeSeparators(QDir::homePath());

    QString displayPath = nativePath;
    bool isHomePath = false;

    if (displayPath.startsWith(homePath)) {
        displayPath.remove(0, homePath.length());

        if (!displayPath.isEmpty() &&
            displayPath.front() != QDir::separator()) {
            displayPath.prepend(QDir::separator());
        }

        displayPath.prepend("~");
        isHomePath = true;
    }

    if (metrics.horizontalAdvance(displayPath) <= width) {
        return displayPath;
    }

    const QFileInfo fileInfo(path);
    const QString directory = QDir::toNativeSeparators(fileInfo.path());
    const QStringList parts = directory.split(QDir::separator(), Qt::SkipEmptyParts);

    QString result = fileInfo.fileName();

    for (qsizetype i = parts.size() - 1; i >= 0; --i) {
        QString candidate = parts[i] + QDir::separator() + result;

        QString prefix;
        if (isHomePath) {
            prefix = "~" + QString(QDir::separator());
        } else {
            prefix = QString(QDir::separator());
        }

        if (metrics.horizontalAdvance(prefix + "..." + QDir::separator() + candidate) > width) {
            break;
        }

        result = candidate;
    }

    QString prefix;

    if (isHomePath) {
        prefix = "~";
    } else {
        #ifdef Q_OS_WIN
        prefix = QFileInfo(path).rootPath();
        #else
        prefix = "/";
        #endif
        prefix = QDir::toNativeSeparators(prefix);
    }

    return prefix + QDir::separator() + "..." + QDir::separator() + result;
}


// ========================================================================== //


} // namespace StringUtil
