#include "util/iconUtil.h"

#include <QGuiApplication>
#include <QPainter>
#include <QSvgRenderer>
#include <QTimer>
#include <QWidget>


// ========================================================================== //


IconManager& IconManager::instance() {
    static IconManager manager;
    return manager;
}

IconManager::IconManager() {
    if (auto* app = QGuiApplication::instance()) {
        app->installEventFilter(this);
    }
}

QIcon IconManager::icon(const QString& path, QPalette::ColorRole role, const QWidget* widget, const QSize& size, IconRotation rotation) {
    const QPalette& palette = widget ? widget->palette() : QGuiApplication::palette();
    const QColor color = palette.color(role);
    const QColor disabledColor = palette.color(QPalette::Disabled, role);

    const CacheKey key {
        .path           = path,
        .color          = color,
        .disabledColor  = disabledColor,
        .size           = size,
        .rotation       = rotation
    };

    if (auto it = mCache.find(key); it != mCache.end()) {
        return it->second;
    }

    QIcon result;
    result.addPixmap(renderIcon(path, color, size, rotation), QIcon::Normal, QIcon::Off);
    result.addPixmap(renderIcon(path, disabledColor, size, rotation), QIcon::Disabled, QIcon::Off);

    mCache.insert_or_assign(key, result);
    return result;
}

bool IconManager::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::ApplicationPaletteChange) {
        IconManager::refresh();
    }
    return QObject::eventFilter(watched, event);
}

void IconManager::refresh() {
    mCache.clear();
    emit iconsChanged();
}

QPixmap IconManager::renderIcon(const QString& path, const QColor& color, const QSize& size, IconRotation rotation) {
    const BaseKey baseKey { path, size };

    QPixmap base = mBaseCache[baseKey];
    if (base.isNull()) {
        QSvgRenderer renderer(path);
        if (!renderer.isValid()) {
            return {};
        }

        renderer.setAspectRatioMode(Qt::KeepAspectRatio);

        QPixmap source(size);
        source.fill(Qt::transparent);

        QPainter painter(&source);
        renderer.render(&painter);

        base = source;
        mBaseCache[baseKey] = base;
    }

    QPixmap tinted(base);
    QPainter tintPainter(&tinted);
    tintPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tintPainter.fillRect(tinted.rect(), color);
    tintPainter.end();

    switch (rotation) {
        case IconRotation::None:         return tinted;
        case IconRotation::Clockwise90:  return tinted.transformed(QTransform().rotate(90));
        case IconRotation::Clockwise180: return tinted.transformed(QTransform().rotate(180));
        case IconRotation::Clockwise270: return tinted.transformed(QTransform().rotate(270));
    }

    std::unreachable();
}

// ========================================================================== //
