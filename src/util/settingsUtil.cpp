#include "util/settingsUtil.h"

#include <QFileInfo>
#include <QStandardPaths>


namespace SettingsUtil {


// ========================================================================== //


namespace /* Anonymous */ {

QString pathKey(PathType type) {
    switch (type) {
    case PathType::Open:   return "openPath";
    case PathType::Save:   return "savePath";
    case PathType::Import: return "importPath";
    case PathType::Export: return "exportPath";
    }
    return {};
}

QSettings& settings() {
    static QSettings sSettings;
    return sSettings;
}

} // namespace Anonymous


// ========================================================================== //


QStringList recentFiles() {
    return settings().value("recentFiles").toStringList();
}

void addRecentFile(const QString& path) {
    auto recentFileList = recentFiles();

    recentFileList.removeAll(path);
    recentFileList.prepend(path);

    while (recentFileList.size() > maxRecentFiles()) {
        recentFileList.removeLast();
    }

    settings().setValue("recentFiles", recentFileList);
}

void removeRecentFile(const QString& path) {
    auto recentFileList = recentFiles();
    recentFileList.removeAll(path);
    settings().setValue("recentFiles", recentFileList);
}

void setRecentFiles(const QStringList& files) {
    settings().setValue("recentFiles", files);
}

QString dialogPath(PathType type) {
    QString path = settings().value(pathKey(type)).toString();

    if (path.isEmpty() && type != PathType::Open) {
        path = settings().value(pathKey(PathType::Open)).toString();
    }

    if (path.isEmpty()) {
        path = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    }

    return path;
}

void setDialogPath(PathType type, const QString& path) {
    settings().setValue(pathKey(type), QFileInfo(path).absolutePath());
}

QByteArray windowGeometry() {
    return settings().value("windowGeometry").toByteArray();
}

void setWindowGeometry(const QByteArray& geometry) {
    settings().setValue("windowGeometry", geometry);
}

QByteArray windowState() {
    return settings().value("windowState").toByteArray();
}

void setWindowState(const QByteArray& state) {
    settings().setValue("windowState", state);
}

QList<QByteArray> splitterStates() {
    QList<QByteArray> states;
    const auto values = settings().value("splitterStates").toList();
    states.reserve(values.size());
    for (const auto& value : values) {
        states.push_back(value.toByteArray());
    }
    return states;
}

void setSplitterStates(const QList<QByteArray>& states) {
    QVariantList values;
    values.reserve(states.size());
    for (const auto& state : states) {
        values.push_back(state);
    }
    settings().setValue("splitterStates", values);
}


// ========================================================================== //


} // namespace SettingsUtil
