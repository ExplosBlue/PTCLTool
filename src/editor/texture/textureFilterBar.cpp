#include "editor/texture/textureFilterBar.h"

#include "ptcl/ptclEnum.h"
#include "util/stringUtil.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QSignalBlocker>
#include <QStringList>

#include <utility>


namespace PtclEditor {


// ========================================================================== //


TextureFilterBar::TextureFilterBar(QWidget* parent) :
    QFrame{parent},
    mFormatGroup{this} {
    setupStyle();
    setupFormatButton();
    setupUnusedOnlyCheck();
    setupSizeCombo();
    setupMaxFileSizeCombo();
    setupClearButton();
    setupLayout();
    setupFormatMenu();
}

void TextureFilterBar::setupStyle() {
    setObjectName("FilterBar");
    rebuildStyle();
}

void TextureFilterBar::rebuildStyle() {
    if (mUpdatingStyle) {
        return;
    }

    mUpdatingStyle = true;

    const QPalette& pal = palette();
    QString style = QString(R"(
    QFrame#FilterBar {
        background-color: %1;
        border: 1px solid %1;
        border-radius: 4px;
    }
)").arg(pal.color(QPalette::Mid).name());
    setStyleSheet(style);

    mUpdatingStyle = false;
}

void TextureFilterBar::changeEvent(QEvent* event) {
    if (event->type() == QEvent::PaletteChange) {
        rebuildStyle();
    }

    QFrame::changeEvent(event);
}

void TextureFilterBar::setupFormatButton() {
    mFormatButton.setText("Format");
    mFormatButton.setPopupMode(QToolButton::InstantPopup);
    mFormatButton.setMenu(&mFormatMenu);
}

void TextureFilterBar::setupUnusedOnlyCheck() {
    mUnusedOnlyCheck.setText("Unused Only");
    mUnusedOnlyCheck.setToolTip("Only show textures that are not referenced by any animation");
    connect(&mUnusedOnlyCheck, &QCheckBox::toggled, this, &TextureFilterBar::unusedOnlyChanged);
}

void TextureFilterBar::setupSizeCombo() {
    const auto options = {
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
        mSizeCombo.addItem(label, value);
    }
    mSizeCombo.setToolTip("Filter textures by their larger dimension.");
    connect(&mSizeCombo, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        emit sizeChanged(mSizeCombo.itemData(index).toInt());
    });
}

void TextureFilterBar::setupMaxFileSizeCombo() {
    const auto options = {
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
        mMaxFileSizeCombo.addItem(text, value);
    }
    mMaxFileSizeCombo.setToolTip("Filter textures by file size.");

    connect(&mMaxFileSizeCombo, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        emit fileSizeChanged(mMaxFileSizeCombo.itemData(index).toInt());
    });
}

void TextureFilterBar::setupClearButton() {
    mClearFiltersButton.setText("Clear Filters");
    mClearFiltersButton.setToolTip("Reset all filters to their defaults");
    connect(&mClearFiltersButton, &QPushButton::clicked, this, &TextureFilterBar::reset);
}

void TextureFilterBar::setupLayout() {
    auto* filterLayout = new QHBoxLayout(this);
    filterLayout->setContentsMargins(8, 4, 8, 4);
    filterLayout->setSpacing(8);
    filterLayout->addWidget(&mFormatButton);

    filterLayout->addSpacing(8);

    filterLayout->addWidget(&mUnusedOnlyCheck);

    filterLayout->addSpacing(8);

    auto* sizeLabel = new QLabel("Size:", this);
    sizeLabel->setToolTip(mSizeCombo.toolTip());
    filterLayout->addWidget(sizeLabel);
    filterLayout->addWidget(&mSizeCombo);

    auto* fileSizeLabel = new QLabel("File Size:", this);
    fileSizeLabel->setToolTip(mMaxFileSizeCombo.toolTip());
    filterLayout->addWidget(fileSizeLabel);
    filterLayout->addWidget(&mMaxFileSizeCombo);

    filterLayout->addStretch(1);
    filterLayout->addWidget(&mClearFiltersButton);
}

void TextureFilterBar::setupFormatMenu() {
    mFormatGroup.setExclusive(false);

    const auto formats = {
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
        auto* action = mFormatMenu.addAction(Ptcl::toString(fmt));
        action->setCheckable(true);
        action->setChecked(true);
        action->setData(static_cast<s32>(fmt));
        mFormatGroup.addAction(action);
    }

    mFormatMenu.addSeparator();
    auto* allAction = mFormatMenu.addAction("Select All");
    auto* noneAction = mFormatMenu.addAction("Select None");

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
        connect(action, &QAction::toggled, this, &TextureFilterBar::updateFormatFilter);
    }

    updateFormatFilter();
}

void TextureFilterBar::updateFormatFilter() {
    const auto actions = mFormatGroup.actions();

    QSet<Ptcl::TextureFormat> enabledFormats;
    for (const auto* action : actions) {
        if (!action->isChecked()) {
            continue;
        }

        enabledFormats.insert(static_cast<Ptcl::TextureFormat>(action->data().toInt()));
    }

    const s32 total = static_cast<s32>(actions.size());

    QStringList enabledNames;
    for (const auto* action : actions) {
        if (action->isChecked()) {
            enabledNames << action->text();
        }
    }

    mFormatButton.setText(QString("Formats (%1/%2)").arg(enabledNames.size()).arg(total));

    if (enabledNames.size() == total) {
        mFormatButton.setToolTip("All formats enabled");
    } else if (enabledNames.isEmpty()) {
        mFormatButton.setToolTip("No formats enabled");
    } else if (enabledNames.size() == 1) {
        mFormatButton.setToolTip(QString("Enabled format: %1").arg(enabledNames.first()));
    } else {
        mFormatButton.setToolTip(QString("Enabled formats: %1").arg(enabledNames.join(", ")));
    }

    emit formatsChanged(enabledFormats);
}

void TextureFilterBar::reset() {
    QSignalBlocker block1{mUnusedOnlyCheck};
    QSignalBlocker block2{mSizeCombo};
    QSignalBlocker block3{mMaxFileSizeCombo};

    const auto actions = mFormatGroup.actions();
    for (auto* action : actions) {
        QSignalBlocker blocker{action};
        action->setChecked(true);
    }

    mUnusedOnlyCheck.setChecked(false);
    mSizeCombo.setCurrentIndex(0);
    mMaxFileSizeCombo.setCurrentIndex(0);

    updateFormatFilter();

    emit unusedOnlyChanged(false);
    emit sizeChanged(0);
    emit fileSizeChanged(0);
}


// ========================================================================== //


} // namespace PtclEditor
