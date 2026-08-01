#include "editor/texture/textureFilterPopup.h"

#include "ptcl/ptclEnum.h"
#include "util/stringUtil.h"

#include <QGuiApplication>
#include <QHideEvent>
#include <QScreen>
#include <QSignalBlocker>
#include <QStringList>

#include <algorithm>
#include <array>
#include <utility>


namespace PtclEditor {


// ========================================================================== //


TextureFilterPopup::TextureFilterPopup(QWidget* parent) :
    StickyMenu{parent} {
    createFormatMenu();
    createSizeMenu();
    createFileSizeMenu();
    createActions();
}

void TextureFilterPopup::popup(const QPoint& anchorGlobal) {
    const QSize hint = sizeHint();

    constexpr s32 gap = 4;
    QPoint pos(anchorGlobal.x() - hint.width() / 2, anchorGlobal.y() - hint.height() - gap);

    const auto* screen = QGuiApplication::screenAt(anchorGlobal);
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }
    if (screen) {
        const QRect geo = screen->availableGeometry();
        pos.setX(std::clamp(pos.x(), geo.left(), std::max(geo.left(), geo.right() - hint.width() + 1)));
        if (pos.y() < geo.top()) {
            pos.setY(anchorGlobal.y() + gap);
        }
        pos.setY(std::clamp(pos.y(), geo.top(), std::max(geo.top(), geo.bottom() - hint.height() + 1)));
    }

    QMenu::popup(pos);
}

void TextureFilterPopup::hideEvent(QHideEvent* event) {
    emit closed();
    QMenu::hideEvent(event);
}

void TextureFilterPopup::createFormatMenu() {
    mFormatMenu = new StickyMenu(this);
    mFormatMenu->setTitle("Format");
    addMenu(mFormatMenu);

    mFormatGroup.setExclusive(false);

    constexpr std::array formats {
        Ptcl::TextureFormat::RGBA8888,
        Ptcl::TextureFormat::RGB888,
        Ptcl::TextureFormat::RGBA5551,
        Ptcl::TextureFormat::RGB565,
        Ptcl::TextureFormat::RGBA4444,
        Ptcl::TextureFormat::LA88,
        Ptcl::TextureFormat::HL8,
        Ptcl::TextureFormat::L8,
        Ptcl::TextureFormat::A8,
        Ptcl::TextureFormat::LA44,
        Ptcl::TextureFormat::L4,
        Ptcl::TextureFormat::A4,
        Ptcl::TextureFormat::ETC1,
        Ptcl::TextureFormat::ETC1_A4,
    };

    for (auto fmt : formats) {
        auto* action = mFormatMenu->addAction(Ptcl::toString(fmt));
        action->setCheckable(true);
        action->setChecked(true);
        action->setData(static_cast<s32>(fmt));
        mFormatGroup.addAction(action);
    }

    mFormatMenu->addSeparator();
    auto* allAction = mFormatMenu->addAction("Select All");
    auto* noneAction = mFormatMenu->addAction("Select None");

    connect(allAction, &QAction::triggered, this, [this] {
        const auto actions = mFormatGroup.actions();
        for (auto* action : actions) {
            action->setChecked(true);
        }
        updateFormatFilter();
    });

    connect(noneAction, &QAction::triggered, this, [this] {
        const auto actions = mFormatGroup.actions();
        for (auto* action : actions) {
            action->setChecked(false);
        }
        updateFormatFilter();
    });

    const auto actions = mFormatGroup.actions();
    for (auto* action : actions) {
        connect(action, &QAction::toggled, this, &TextureFilterPopup::updateFormatFilter);
    }

    updateFormatFilter();
}

void TextureFilterPopup::createActions() {
    mUnusedOnlyAction.setText("Unused Only");
    mUnusedOnlyAction.setToolTip("Only show textures that are not referenced by any animation");
    mUnusedOnlyAction.setCheckable(true);
    addAction(&mUnusedOnlyAction);

    connect(&mUnusedOnlyAction, &QAction::toggled, this, [this](bool checked) {
        mState.unusedOnly = checked;
        emit filterChanged(mState);
    });

    addSeparator();
    QAction* clearAction = addAction("Clear Filters");
    connect(clearAction, &QAction::triggered, this, &TextureFilterPopup::reset);
}

void TextureFilterPopup::createSizeMenu() {
    mSizeMenu = new StickyMenu(this);
    mSizeMenu->setTitle("Dimensions");
    addMenu(mSizeMenu);
    mSizeMenu->setToolTip("Filter textures by their larger dimension.");

    mSizeGroup.setExclusive(true);

    const std::array options {
        std::pair{QString("Any"), 0},
        std::pair{QString("Up to 8 px"), 8},
        std::pair{QString("Up to 16 px"), 16},
        std::pair{QString("Up to 32 px"), 32},
        std::pair{QString("Up to 64 px"), 64},
        std::pair{QString("Up to 128 px"), 128},
        std::pair{QString("Up to 256 px"), 256},
        std::pair{QString("Up to 512 px"), 512},
    };

    for (const auto& [label, value] : options) {
        QAction* action = mSizeMenu->addAction(label);
        action->setCheckable(true);
        action->setData(value);
        mSizeGroup.addAction(action);
    }

    mSizeGroup.actions().constFirst()->setChecked(true);

    connect(&mSizeGroup, &QActionGroup::triggered, this, [this](QAction* action) {
        mState.maxDimension = action->data().toInt();
        updateSizeMenuLabel();
        emit filterChanged(mState);
    });

    updateSizeMenuLabel();
}

void TextureFilterPopup::createFileSizeMenu() {
    mFileSizeMenu = new StickyMenu(this);
    mFileSizeMenu->setTitle("File Size");
    addMenu(mFileSizeMenu);
    mFileSizeMenu->setToolTip("Filter textures by file size.");

    mFileSizeGroup.setExclusive(true);

    const std::array options {
        std::pair{QString("Any"), 0LL},
        std::pair{QString(), 64LL},
        std::pair{QString(), 256LL},
        std::pair{QString(), 1024LL},
        std::pair{QString(), 4LL * 1024},
        std::pair{QString(), 16LL * 1024},
        std::pair{QString(), 64LL * 1024},
        std::pair{QString(), 256LL * 1024},
        std::pair{QString(), 1024LL * 1024},
    };

    for (const auto& [label, value] : options) {
        const QString text = label.isEmpty()
            ? QString("Up to %1").arg(StringUtil::formatBytes(value))
            : label;
        QAction* action = mFileSizeMenu->addAction(text);
        action->setCheckable(true);
        action->setData(value);
        mFileSizeGroup.addAction(action);
    }

    mFileSizeGroup.actions().constFirst()->setChecked(true);

    connect(&mFileSizeGroup, &QActionGroup::triggered, this, [this](QAction* action) {
        mState.maxFileSize = action->data().toLongLong();
        updateFileSizeMenuLabel();
        emit filterChanged(mState);
    });

    updateFileSizeMenuLabel();
}

void TextureFilterPopup::updateSizeMenuLabel() {
    const QAction* selected = mSizeGroup.checkedAction();
    const s32 value = selected ? selected->data().toInt() : 0;
    if (value != 0) {
        mSizeMenu->setTitle(QString("Dimensions (Up to %1 px)").arg(value));
    } else {
        mSizeMenu->setTitle("Dimensions");
    }
}

void TextureFilterPopup::updateFileSizeMenuLabel() {
    const QAction* selected = mFileSizeGroup.checkedAction();
    if (selected && selected->data().toInt() != 0) {
        mFileSizeMenu->setTitle(QString("File Size (%1)").arg(selected->text()));
    } else {
        mFileSizeMenu->setTitle("File Size");
    }
}

void TextureFilterPopup::updateFormatFilter() {
    const auto actions = mFormatGroup.actions();
    QSet<Ptcl::TextureFormat> enabledFormats;
    for (const auto* action : actions) {
        if (!action->isChecked()) {
            continue;
        }

        enabledFormats.insert(static_cast<Ptcl::TextureFormat>(action->data().toInt()));
    }

    const s32 total = static_cast<s32>(actions.size());
    const s32 enabled = static_cast<s32>(enabledFormats.size());

    if (enabled == 0) {
        mFormatMenu->setTitle("Formats (None)");
    } else if (enabled != total) {
        mFormatMenu->setTitle(QString("Formats (%1/%2)").arg(enabled).arg(total));
    } else {
        mFormatMenu->setTitle("Formats");
    }

    mState.formats = enabledFormats;
    emit filterChanged(mState);
}

void TextureFilterPopup::reset() {
    const auto actions = mFormatGroup.actions();
    for (auto* action : actions) {
        QSignalBlocker blocker{action};
        action->setChecked(true);
    }

    {
        QSignalBlocker blocker{&mUnusedOnlyAction};
        mUnusedOnlyAction.setChecked(false);
    }

    mSizeGroup.actions().constFirst()->setChecked(true);
    mFileSizeGroup.actions().constFirst()->setChecked(true);

    mState.unusedOnly = false;
    mState.maxDimension = 0;
    mState.maxFileSize = 0;

    updateSizeMenuLabel();
    updateFileSizeMenuLabel();
    updateFormatFilter();
}


// ========================================================================== //


} // namespace PtclEditor
