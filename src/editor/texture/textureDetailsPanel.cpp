#include "editor/texture/textureDetailsPanel.h"

#include "ptcl/ptclDocument.h"
#include "util/stringUtil.h"

#include <QEvent>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPixmap>
#include <QPushButton>
#include <QTreeWidgetItem>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


TextureDetailsPanel::TextureDetailsPanel(QWidget* parent) :
    QWidget{parent} {
    setupDetailsPage();
    setupUsersPage();
    setupTabs();
    setupConnections();

    setEnabled(false);
}

void TextureDetailsPanel::setupDetailsPage() {
    mThumbnailWidget.setThumbnailSize({256, 256});
    mThumbnailWidget.setMinimumSize({64, 64});
    mExportButton.setText("Export");
    mReplaceButton.setText("Replace");
    mDeleteButton.setText("Delete");

    auto* detailsLayout = new QVBoxLayout(&mDetailsPage);
    detailsLayout->setContentsMargins(10, 10, 10, 10);
    detailsLayout->setSpacing(6);

    detailsLayout->addWidget(&mThumbnailWidget);

    detailsLayout->addWidget(&mNameLabel);
    detailsLayout->addWidget(&mFormatLabel);
    detailsLayout->addWidget(&mDimensionsLabel);
    detailsLayout->addWidget(&mSizeLabel);

    auto* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(&mExportButton);
    buttonLayout->addWidget(&mReplaceButton);
    buttonLayout->addWidget(&mDeleteButton);
    detailsLayout->addLayout(buttonLayout);
    detailsLayout->addStretch(1);
}

void TextureDetailsPanel::setupUsersPage() {
    mUsersList.setMouseTracking(true);
    mUsersList.setAlternatingRowColors(true);
    mUsersList.setRootIsDecorated(false);
    mUsersList.setSelectionMode(QAbstractItemView::SingleSelection);
    mUsersList.setSelectionBehavior(QAbstractItemView::SelectRows);
    mUsersList.setColumnCount(3);
    mUsersList.setHeaderLabels({"Set", "Emitter", "Child"});
    mUsersList.header()->setStretchLastSection(false);
    mUsersList.header()->setSectionResizeMode(QHeaderView::Interactive);
    mUsersList.viewport()->installEventFilter(this);

    mUsersEmptyLabel.setEnabled(false);
    mUsersEmptyLabel.setAlignment(Qt::AlignCenter);

    mUsersStack.addWidget(&mUsersList);
    mUsersStack.addWidget(&mUsersEmptyLabel);

    auto* usersLayout = new QVBoxLayout(&mUsersPage);
    usersLayout->setContentsMargins(10, 10, 10, 10);
    usersLayout->setSpacing(6);
    usersLayout->addWidget(&mUsersHeaderLabel);
    usersLayout->addWidget(&mUsersStack, 1);
}

void TextureDetailsPanel::setupTabs() {
    mTabs.setTabPosition(QTabWidget::South);
    mTabs.addTab(&mDetailsPage, "Details");
    mTabs.addTab(&mUsersPage, "Users");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(&mTabs);
}

void TextureDetailsPanel::setupConnections() {
    connect(&mExportButton, &QPushButton::clicked, this, [this] {
        if (mTexturePtr) {
            emit exportRequested(mTexturePtr);
        }
    });

    connect(&mReplaceButton, &QPushButton::clicked, this, [this] {
        if (mIndex.isValid()) {
            emit replaceRequested(mIndex);
        }
    });

    connect(&mDeleteButton, &QPushButton::clicked, this, [this] {
        if (mIndex.isValid()) {
            emit deleteRequested(mIndex);
        }
    });

    connect(&mUsersList, &QTreeWidget::itemClicked, this, &TextureDetailsPanel::navigateToUser);
    connect(&mUsersList, &QTreeWidget::itemActivated, this, &TextureDetailsPanel::navigateToUser);
}

void TextureDetailsPanel::setDocument(Ptcl::Document* document) {
    mDocument = document;
    updateUsers();
}

void TextureDetailsPanel::setSelection(Ptcl::Selection* selection) {
    mSelection = selection;
}

void TextureDetailsPanel::clearDetails() {
    mThumbnailWidget.clear();
    mNameLabel.clear();
    mFormatLabel.clear();
    mDimensionsLabel.clear();
    mSizeLabel.clear();

    updateUsers();

    setEnabled(false);
}

void TextureDetailsPanel::setTexture(const QModelIndex& index, Ptcl::Texture* texture) {
    mTexturePtr = texture;
    mIndex = index;

    if (!mTexturePtr) {
        clearDetails();
        return;
    }

    setEnabled(true);
    refresh();
}

void TextureDetailsPanel::refresh() {
    if (!mTexturePtr) {
        return;
    }

    mThumbnailWidget.setPixmap(QPixmap::fromImage(mTexturePtr->textureData()));

    updateLabels();
    updateUsers();
}

void TextureDetailsPanel::refreshUsers() {
    updateUsers();
}

bool TextureDetailsPanel::eventFilter(QObject* watched, QEvent* event) {
    if (watched == mUsersList.viewport()) {
        if (event->type() == QEvent::MouseMove) {
            const auto* mouseEvent = static_cast<const QMouseEvent*>(event);
            const bool overItem = mUsersList.itemAt(mouseEvent->position().toPoint()) != nullptr;
            mUsersList.viewport()->setCursor(overItem ? Qt::PointingHandCursor : Qt::ArrowCursor);
        } else if (event->type() == QEvent::Leave) {
            mUsersList.viewport()->setCursor(Qt::ArrowCursor);
        }
    }

    return QWidget::eventFilter(watched, event);
}

void TextureDetailsPanel::updateLabels() {
    if (!mTexturePtr) {
        return;
    }

    const auto& img = mTexturePtr->textureData();

    auto setLabel = [](QLabel& label, const QString& title, const QString& value) {
        label.setText(QString("<b>%1</b>: %2").arg(title, value));
    };

    setLabel(mNameLabel, "Texture", QString::number(mIndex.row()));
    setLabel(mFormatLabel, "Format", Ptcl::toString(mTexturePtr->textureFormat()));
    setLabel(mDimensionsLabel, "Dimensions", QString("%1 x %2").arg(img.width()).arg(img.height()));
    setLabel(mSizeLabel, "Size", StringUtil::formatBytes(mTexturePtr->textureDataRaw().size()));
}

void TextureDetailsPanel::addUsageItem(const Ptcl::TextureUsage& usage) {
    const auto* set = mDocument->emitterSet(usage.setIndex);
    const auto* emitter = mDocument->emitter(usage.setIndex, usage.emitterIndex);

    const auto& setText = set->name();
    const auto& emitterText = emitter->name();

    auto* item = new QTreeWidgetItem(&mUsersList);

    item->setText(0, setText);
    item->setText(1, emitterText);

    item->setText(2, usage.isChild ? QString::fromUtf8("✓") : "");

    const auto toolTip = QString("Set %1: %2 — Emitter %3: %4 (%5)")
        .arg(usage.setIndex).arg(setText)
        .arg(usage.emitterIndex).arg(emitterText)
        .arg(usage.isChild ? "child" : "main");

    for (s32 i = 0; i < 3; ++i) {
        item->setToolTip(i, toolTip);
    }

    item->setData(0, SetIndexRole, usage.setIndex);
    item->setData(0, EmitterIndexRole, usage.emitterIndex);
    item->setData(0, ChildRole, usage.isChild);
}

void TextureDetailsPanel::showEmptyUsers() {
    mUsersEmptyLabel.setText("Not used by any emitter");
    mUsersStack.setCurrentWidget(&mUsersEmptyLabel);
}

void TextureDetailsPanel::setUsersCount(s32 count) {
    mUsersHeaderLabel.setText(QString("<b>Users</b>: %1").arg(count));
}

void TextureDetailsPanel::updateUsers() {
    mUsersList.clear();

    const auto usages = (mTexturePtr && mDocument)
        ? mDocument->textureUsages(mTexturePtr)
        : std::vector<Ptcl::TextureUsage>{};

    setUsersCount(static_cast<s32>(usages.size()));

    if (usages.empty()) {
        showEmptyUsers();
        return;
    }

    mUsersStack.setCurrentWidget(&mUsersList);

    for (const auto& usage : usages) {
        addUsageItem(usage);
    }
}

void TextureDetailsPanel::navigateToUser(QTreeWidgetItem* item) {
    if (!mSelection || !item) {
        return;
    }

    const s32 setIndex = item->data(0, SetIndexRole).toInt();
    const s32 emitterIndex = item->data(0, EmitterIndexRole).toInt();
    const bool isChild = item->data(0, ChildRole).toBool();

    mSelection->set(
        setIndex,
        emitterIndex,
        isChild ? Ptcl::Selection::Type::EmitterChild : Ptcl::Selection::Type::Emitter
    );
}


// ========================================================================== //


} // namespace PtclEditor
