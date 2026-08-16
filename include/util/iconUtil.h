#pragma once

#include <QColor>
#include <QEvent>
#include <QIcon>
#include <QObject>
#include <QPalette>
#include <QPixmap>
#include <QSize>
#include <QString>

#include <unordered_map>


// ========================================================================== //

enum class IconRotation {
    None         = 0,
    Clockwise90  = 90,
    Clockwise180 = 180,
    Clockwise270 = 270,
};

class IconManager final : public QObject {
    Q_OBJECT
public:
    static IconManager& instance();

    IconManager(const IconManager&) = delete;
    IconManager& operator=(const IconManager&) = delete;

    QIcon icon(const QString& path, QPalette::ColorRole role, const QWidget* widget, const QSize& size, IconRotation rotation);

    void refresh();

signals:
    void iconsChanged();

private:
    IconManager();
    ~IconManager() final = default;

    bool eventFilter(QObject* watched, QEvent* event) final;

    QPixmap renderIcon(const QString& path, const QColor& color, const QSize& size, IconRotation rotation);

private:
    struct CacheKey {
        QString path;
        QColor color;
        QSize size;
        IconRotation rotation;

        bool operator==(const CacheKey&) const = default;
    };

    struct CacheKeyHash {
        std::size_t operator()(const CacheKey& key) const noexcept {
            std::size_t hash = 0;

            auto combine = [&hash](std::size_t value) {
                hash ^= value + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            };

            combine(qHash(key.path));
            combine(qHash(key.color.rgba()));
            combine(qHash(key.size));
            combine(qHash(key.rotation));

            return hash;
        }
    };

    std::unordered_map<CacheKey, QIcon, CacheKeyHash> mCache;
};


// ========================================================================== //


namespace IconUtil {

template <typename T>
concept Iconable = requires(T* object, const QIcon& icon) {
    object->setIcon(icon);
};

template <typename Iconable>
void setIcon(Iconable* object, const QString& name, QWidget* context, const QSize& size, IconRotation rotation = IconRotation::None) {
    object->setIcon(IconManager::instance().icon(
        QStringLiteral(":/res/icons/%1.svg").arg(name),
        QPalette::Text,
        context,
        size,
        rotation
    ));
}


} // namespace IconUtil


// ========================================================================== //

