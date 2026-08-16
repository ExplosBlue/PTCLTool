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
    const auto& color = palette.color(role);

    const CacheKey key {
        .path     = path,
        .color    = color,
        .size     = size,
        .rotation = rotation
    };

    if (auto it = mCache.find(key); it != mCache.end()) {
        return it->second;
    }

    QIcon result = renderIcon(path, color, size, rotation);

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
    QSvgRenderer renderer(path);
    if (!renderer.isValid()) {
        return {};
    }

    renderer.setAspectRatioMode(Qt::KeepAspectRatio);

    QPixmap source(size);
    source.fill(Qt::transparent);

    QPainter painter(&source);
    renderer.render(&painter);

    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(source.rect(), color);

    switch (rotation) {
        case IconRotation::None:         return source;
        case IconRotation::Clockwise90:  return source.transformed(QTransform().rotate(90));
        case IconRotation::Clockwise180: return source.transformed(QTransform().rotate(180));
        case IconRotation::Clockwise270: return source.transformed(QTransform().rotate(270));
    }

    std::unreachable();
}

// ========================================================================== //
