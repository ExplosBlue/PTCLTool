#include "util/settingsUtil.h"

#include <QFileInfo>
#include <QStandardPaths>

#include <algorithm>
#include <bit>
#include <cmath>


namespace SettingsUtil {


// ========================================================================== //


namespace /* Anonymous */ {

QString pathKey(PathType type) {
    switch (type) {
    case PathType::Open:             return "openPath";
    case PathType::Save:             return "savePath";
    case PathType::ImportTexture:    return "importTexPath";
    case PathType::ExportTexture:    return "exportTexPath";
    case PathType::ExportProject:    return "exportProjPath";
    case PathType::ImportEmitter:    return "importEmitterPath";
    case PathType::ImportEmitterSet: return "importEmitterSetPath";
    case PathType::ExportEmitter:    return "exportEmitterPath";
    case PathType::ExportEmitterSet: return "exportEmitterSetPath";
    }
    return {};
}

QSettings& settings() {
    static QSettings sSettings;
    return sSettings;
}

f32 sanitizeGridCellSize(f32 size) {
    if (!std::isfinite(size)) {
        return cViewportGridCellSizeDefault;
    }
    return std::clamp(size, cViewportGridCellSizeMin, cViewportGridCellSizeMax);
}

s32 sanitizeRegularParticleCapacity(s32 capacity) {
    capacity = std::clamp(
        capacity,
        cRegularParticleCapacityMin,
        cRegularParticleCapacityMax
    );
    return static_cast<s32>(std::bit_ceil(static_cast<u32>(capacity)));
}

s32 sanitizeCompactParticleCapacity(s32 capacity) {
    capacity = std::clamp(
        capacity,
        cCompactParticleCapacityMin,
        cCompactParticleCapacityMax
    );
    return static_cast<s32>(std::bit_ceil(static_cast<u32>(capacity)));
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

bool viewportOrthographic() {
    return settings().value("viewport/orthographic", true).toBool();
}

void setViewportOrthographic(bool orthographic) {
    settings().setValue("viewport/orthographic", orthographic);
}

bool viewportGridVisible() {
    return settings().value("viewport/gridVisible", cViewportGridVisibleDefault).toBool();
}

void setViewportGridVisible(bool visible) {
    settings().setValue("viewport/gridVisible", visible);
}

f32 viewportGridCellSize() {
    const f32 size = settings().value("viewport/gridCellSize", cViewportGridCellSizeDefault).toFloat();
    return sanitizeGridCellSize(size);
}

void setViewportGridCellSize(f32 size) {
    settings().setValue("viewport/gridCellSize", sanitizeGridCellSize(size));
}

QColor defaultViewportBackgroundColor() {
    return QColor::fromRgbF(0.094f, 0.094f, 0.388f);
}

QColor viewportBackgroundColor() {
    const QColor color = settings().value("viewport/backgroundColor", defaultViewportBackgroundColor()).value<QColor>();

    if (!color.isValid()) {
        return defaultViewportBackgroundColor();
    }

    QColor opaqueColor = color.toRgb();
    opaqueColor.setAlpha(255);
    return opaqueColor;
}

void setViewportBackgroundColor(const QColor& color) {
    if (!color.isValid()) {
        return;
    }

    QColor opaqueColor = color.toRgb();
    opaqueColor.setAlpha(255);
    settings().setValue("viewport/backgroundColor", opaqueColor);
}

s32 regularParticleCapacity() {
    const s32 capacity = settings().value("viewport/regularParticleCapacity", cRegularParticleCapacityDefault).toInt();
    return sanitizeRegularParticleCapacity(capacity);
}

void setRegularParticleCapacity(s32 capacity) {
    settings().setValue("viewport/regularParticleCapacity", sanitizeRegularParticleCapacity(capacity));
}

s32 compactParticleCapacity() {
    const s32 capacity = settings().value("viewport/compactParticleCapacity", cCompactParticleCapacityDefault).toInt();
    return sanitizeCompactParticleCapacity(capacity);
}

void setCompactParticleCapacity(s32 capacity) {
    settings().setValue("viewport/compactParticleCapacity", sanitizeCompactParticleCapacity(capacity));
}

f32 viewportPlaybackSpeed() {
    return settings().value("viewport/playbackSpeed", 1.0f).toFloat();
}

void setViewportPlaybackSpeed(f32 speed) {
    settings().setValue("viewport/playbackSpeed", speed);
}

bool viewportLooping() {
    return settings().value("viewport/looping", true).toBool();
}

void setViewportLooping(bool looping) {
    settings().setValue("viewport/looping", looping);
}


// ========================================================================== //


} // namespace SettingsUtil
