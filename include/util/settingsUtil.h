#ifndef SETTINGSUTIL_H
#define SETTINGSUTIL_H

#include <QSettings>


namespace SettingsUtil {

class SettingsMgr {
public:
    static SettingsMgr& instance() {
        static SettingsMgr sInstance;
        return sInstance;
    }

    SettingsMgr(const SettingsMgr&) = delete;
    SettingsMgr& operator=(const SettingsMgr&) = delete;

    QStringList recentFiles() const;
    void addRecentFile(const QString& path);

    inline constexpr int maxRecentFiles() { return sMaxRecentFiles; }

    QString lastOpenPath() const;
    void setLastOpenPath(const QString& path);

    QString lastSavePath() const;
    void setLastSavePath(const QString& path);

private:
    SettingsMgr() = default;

    static constexpr int sMaxRecentFiles = 5;

private:
    QSettings mSettings;
};

} // namespace SettingsUtil


#endif // SETTINGSUTIL_H
