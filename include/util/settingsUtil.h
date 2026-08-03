#pragma once

#include "typedefs.h"

#include <QSettings>


namespace SettingsUtil {


// ========================================================================== //


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
    void removeRecentFile(const QString& path);
    void setRecentFiles(const QStringList& files);

    inline constexpr s32 maxRecentFiles() { return sMaxRecentFiles; }

    QString lastOpenPath() const;
    void setLastOpenPath(const QString& path);

    QString lastSavePath() const;
    void setLastSavePath(const QString& path);

    QString lastImportPath() const;
    void setLastImportPath(const QString& path);

    QString lastExportPath() const;
    void setLastExportPath(const QString& path);

    QByteArray windowGeometry() const;
    void setWindowGeometry(const QByteArray& geometry);

    QByteArray windowState() const;
    void setWindowState(const QByteArray& state);

    QList<QByteArray> splitterStates() const;
    void setSplitterStates(const QList<QByteArray>& states);

private:
    SettingsMgr() = default;

    static constexpr s32 sMaxRecentFiles = 5;

private:
    QSettings mSettings;
};


// ========================================================================== //


} // namespace SettingsUtil
