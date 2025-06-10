#include "util/settingsUtil.h"


namespace SettingsUtil {


// ========================================================================== //


QStringList SettingsMgr::recentFiles() const {
    return mSettings.value("recentFiles").toStringList();
}

void SettingsMgr::addRecentFile(const QString& path) {
    auto recentFileList = recentFiles();

    recentFileList.removeAll(path);
    recentFileList.prepend(path);

    while (recentFileList.size() > sMaxRecentFiles) {
        recentFileList.removeLast();
    }

    mSettings.setValue("recentFiles", recentFileList);
}

QString SettingsMgr::lastOpenPath() const {
    return mSettings.value("openPath").toString();
}

void SettingsMgr::setLastOpenPath(const QString& path) {
    mSettings.setValue("openPath", path);
}

QString SettingsMgr::lastSavePath() const {
    return mSettings.value("savePath").toString();
}

void SettingsMgr::setLastSavePath(const QString& path) {
    mSettings.setValue("savePath", path);
}

QString SettingsMgr::lastImportPath() const {
    return mSettings.value("importPath").toString();
}

void SettingsMgr::setLastImportPath(const QString& path) {
    mSettings.setValue("importPath", path);
}

QString SettingsMgr::lastExportPath() const {
    return mSettings.value("exportPath").toString();
}

void SettingsMgr::setLastExportPath(const QString& path) {
    mSettings.setValue("exportPath", path);
}

QByteArray SettingsMgr::windowGeometry() const {
    return mSettings.value("windowGeometry").toByteArray();
};

void SettingsMgr::setWindowGeometry(const QByteArray& geometry) {
    mSettings.setValue("windowGeometry", geometry);
}

QByteArray SettingsMgr::windowState() const {
    return mSettings.value("windowState").toByteArray();
};

void SettingsMgr::setWindowState(const QByteArray& state) {
    mSettings.setValue("windowState", state);
}


// ========================================================================== //


} // namespace SettingsUtil
