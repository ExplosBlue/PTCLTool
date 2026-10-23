#pragma once

#include "typedefs.h"

#include <QColor>
#include <QSettings>


namespace SettingsUtil {


// ========================================================================== //


inline constexpr f32 cViewportGridCellSizeMin = 0.01f;
inline constexpr f32 cViewportGridCellSizeMax = 10000.0f;
inline constexpr f32 cViewportGridCellSizeDefault = 1.0f;

inline constexpr bool cViewportGridVisibleDefault = true;

inline constexpr s32 cRegularParticleCapacityMin = 64;
inline constexpr s32 cRegularParticleCapacityMax = 8192;
inline constexpr s32 cRegularParticleCapacityDefault = 256;

inline constexpr s32 cCompactParticleCapacityMin = 128;
inline constexpr s32 cCompactParticleCapacityMax = 65536;
inline constexpr s32 cCompactParticleCapacityDefault = 1024;


// ========================================================================== //


enum class PathType {
    Open,
    Save,
    ImportTexture,
    ExportTexture,
    ExportProject,
    ImportEmitter,
    ImportEmitterSet,
    ExportEmitter,
    ExportEmitterSet,
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

bool viewportOrthographic();
void setViewportOrthographic(bool orthographic);

bool viewportGridVisible();
void setViewportGridVisible(bool visible);

f32 viewportGridCellSize();
void setViewportGridCellSize(f32 size);

QColor defaultViewportBackgroundColor();
QColor viewportBackgroundColor();
void setViewportBackgroundColor(const QColor& color);

s32 regularParticleCapacity();
void setRegularParticleCapacity(s32 capacity);

s32 compactParticleCapacity();
void setCompactParticleCapacity(s32 capacity);

f32 viewportPlaybackSpeed();
void setViewportPlaybackSpeed(f32 speed);

bool viewportLooping();
void setViewportLooping(bool looping);


// ========================================================================== //


} // namespace SettingsUtil
