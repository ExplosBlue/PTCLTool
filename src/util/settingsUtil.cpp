#include "util/settingsUtil.h"

namespace SettingsUtil {


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


} // namespace SettingsUtil
