#pragma once

#include "typedefs.h"

#include <QSettings>


namespace SettingsUtil {


// ========================================================================== //


enum class PathType {
    Open,
    Save,
    ImportTexture,
    ExportTexture,
    ExportProject,
};


// ========================================================================== //


QStringList recentFiles();
void addRecentFile(const QString& path);
void removeRecentFile(const QString& path);
void setRecentFiles(const QStringList& files);

constexpr s32 maxRecentFiles() { return 5; }

QString dialogPath(PathType type);
void setDialogPath(PathType type, const QString& path);

QByteArray windowGeometry();
void setWindowGeometry(const QByteArray& geometry);

QByteArray windowState();
void setWindowState(const QByteArray& state);

QList<QByteArray> splitterStates();
void setSplitterStates(const QList<QByteArray>& states);


// ========================================================================== //


} // namespace SettingsUtil
