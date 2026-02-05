#include "editor/textureListWidget.h"
#include "editor/textureImportDialog.h"
#include "editor/components/thumbnailWidget.h"

#include "ptcl/ptcl.h"
#include "util/settingsUtil.h"

#include <QContextMenuEvent>
#include <QFileDialog>
#include <QMenu>
#include <QPainter>
#include <QStandardItemModel>
#include <QStandardPaths>


namespace PtclEditor {


// ========================================================================== //


TextureListItem::TextureListItem(const QString& text, QIcon thumbnail, QWidget* parent) :
    QWidget{parent}, mThumbnail{std::move(thumbnail)}, mHovered{false} {
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);

    auto* layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(5, 5, 5, 5);

    auto* thumbnailWidget = new ThumbnailWidget(this);
    thumbnailWidget->setPixmap(mThumbnail.pixmap(64, 64));
    thumbnailWidget->setThumbnailSize(QSize(64, 64));
    layout->addWidget(thumbnailWidget);

    auto* textLabel = new QLabel(text, this);
    textLabel->setWordWrap(true);
    layout->addWidget(textLabel);

    // Export Action
    mExportAction.setText("Export");
    mExportAction.setIcon(QIcon(":/res/icons/export_image.png"));
    connect(&mExportAction, &QAction::triggered, this, [this](){ emit exportImage(); });

    // Replace Action
    mReplaceAction.setText("Replace");
    mReplaceAction.setIcon(QIcon(":/res/icons/replace_image.png"));
    connect(&mReplaceAction, &QAction::triggered, this, [this](){ emit replaceTexture(); });

    setLayout(layout);
}

void TextureListItem::enterEvent(QEnterEvent* event) {
    mHovered = true;
    update();
    QWidget::enterEvent(event);
}

void TextureListItem::leaveEvent(QEvent* event) {
    mHovered = false;
    update();
    QWidget::leaveEvent(event);
}

void TextureListItem::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    if (mHovered) {
        QStyleOption opt;
        painter.fillRect(rect(), opt.palette.color(QPalette::Highlight));
    }

    QWidget::paintEvent(event);
}

void TextureListItem::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);
    menu.addAction(&mExportAction);
    menu.addAction(&mReplaceAction);
    menu.exec(event->globalPos());
}


// ========================================================================== //


TextureListWidget::TextureListWidget(QWidget *parent) :
    QWidget{parent}, mTexturesPtr{nullptr}, mScrollArea{this},
    mLastColumnCount{-1}, mLastWidgetCount{0} {
    auto* mainLayout = new QVBoxLayout;

    // Grid Container
    mGridLayout.setSpacing(10);
    mGridLayout.setContentsMargins(10, 10, 10, 10);
    mGridContainer.setLayout(&mGridLayout);

    // Export All
    mActionExportAll.setText("Export All");
    mActionExportAll.setIcon(QIcon(":/res/icons/export_image.png"));
    connect(&mActionExportAll, &QAction::triggered, this, &TextureListWidget::exportAll);

    // Import Texture
    mActionImportTexture.setText("Import Texture");
    mActionImportTexture.setIcon(QIcon(":/res/icons/import_image.png"));
    connect(&mActionImportTexture, &QAction::triggered, this, &TextureListWidget::importTexture);

    // Toolbar
    mToolbar.addAction(&mActionExportAll);
    mToolbar.addAction(&mActionImportTexture);
    mToolbar.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mainLayout->addWidget(&mToolbar);

    // Scroll Area
    mScrollArea.setWidgetResizable(true);
    mScrollArea.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mScrollArea.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mScrollArea.setWidget(&mGridContainer);
    mainLayout->addWidget(&mScrollArea);

    setLayout(mainLayout);
}

void TextureListWidget::setTextures(Ptcl::TextureList* textures) {
    if (mTexturesPtr == textures) {
        return;
    }

    mTexturesPtr = textures;
    populateList();
}

void TextureListWidget::exportAll() {
    if (!mTexturesPtr) {
        return;
    }

    QString basePath = SettingsUtil::SettingsMgr::instance().lastExportPath();
    if (basePath.isEmpty()) {
        QString lastOpenPath = SettingsUtil::SettingsMgr::instance().lastOpenPath();
        if (!lastOpenPath.isEmpty()) {
            basePath = lastOpenPath;
        } else {
            basePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
    }

    QString dirPath = QFileDialog::getExistingDirectory(this, "Export textures", basePath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (dirPath.isEmpty()) {
        return;
    }

    for (int index = 0; index < mTexturesPtr->size(); ++index) {
        const auto& texture = (*mTexturesPtr)[index];
        QString filePath = QString("%1/tex_%2.png").arg(dirPath).arg(index);
        texture->textureData().save(filePath);
    }

    SettingsUtil::SettingsMgr::instance().setLastExportPath(QFileInfo(dirPath).absolutePath());
}

void TextureListWidget::exportImage() {
    if (!mTexturesPtr) {
        return;
    }

    auto* item = qobject_cast<TextureListItem*>(sender());
    if (!item) {
        return;
    }

    QString basePath = SettingsUtil::SettingsMgr::instance().lastExportPath();
    if (basePath.isEmpty()) {
        QString lastOpenPath = SettingsUtil::SettingsMgr::instance().lastOpenPath();
        if (!lastOpenPath.isEmpty()) {
            basePath = lastOpenPath;
        } else {
            basePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
    }

    QString filePath = QFileDialog::getSaveFileName(this, "Export texture", basePath, "*.png");

    if (filePath.isEmpty()) {
        return;
    }

    if (!filePath.endsWith(".png")) {
        filePath.append(".png");
    }

    bool ok = false;
    int index = item->property("textureIndex").toInt(&ok);

    if (!ok) {
        qWarning() << "textureIndex propery missing or invalid";
        return;
    }

    const auto& texture = (*mTexturesPtr)[index];
    texture->textureData().save(filePath);

    SettingsUtil::SettingsMgr::instance().setLastExportPath(QFileInfo(filePath).absolutePath());
}

void TextureListWidget::importTexture() {
    if (!mTexturesPtr) {
        return;
    }

    QString basePath = SettingsUtil::SettingsMgr::instance().lastImportPath();
    if (basePath.isEmpty()) {
        QString lastOpenPath = SettingsUtil::SettingsMgr::instance().lastOpenPath();
        if (!lastOpenPath.isEmpty()) {
            basePath = lastOpenPath;
        } else {
            basePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
    }

    QString filePath = QFileDialog::getOpenFileName(this, "Import texture", basePath, "*.png");

    if (filePath.isEmpty()) {
        return;
    }

    TextureImportDialog importDialog(this);
    importDialog.setFilePath(filePath);

    if (importDialog.exec() == QDialog::Accepted) {
        mTexturesPtr->push_back(std::move(importDialog.getTexture()));
        int index = static_cast<int>(mTexturesPtr->size()) - 1;
        setupListItem(nullptr, index);
        relayoutGrid();
    }

    SettingsUtil::SettingsMgr::instance().setLastImportPath(QFileInfo(filePath).absolutePath());
}

void TextureListWidget::replaceTexture() {
    if (!mTexturesPtr) {
        return;
    }

    auto* item = qobject_cast<TextureListItem*>(sender());
    if (!item) {
        return;
    }

    bool ok = false;
    int index = item->property("textureIndex").toInt(&ok);

    if (!ok) {
        qWarning() << "textureIndex propery missing or invalid";
        return;
    }

    QString basePath = SettingsUtil::SettingsMgr::instance().lastImportPath();
    if (basePath.isEmpty()) {
        QString lastOpenPath = SettingsUtil::SettingsMgr::instance().lastOpenPath();
        if (!lastOpenPath.isEmpty()) {
            basePath = lastOpenPath;
        } else {
            basePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        }
    }

    QString filePath = QFileDialog::getOpenFileName(this, "Import texture", basePath, "*.png");

    if (filePath.isEmpty()) {
        return;
    }

    TextureImportDialog importDialog(this);
    importDialog.setFilePath(filePath);

    if (importDialog.exec() == QDialog::Accepted) {
        const auto& texture = (*mTexturesPtr)[index];
        texture->replaceTexture(*importDialog.getTexture());
        updateItemAt(index);
    }

    SettingsUtil::SettingsMgr::instance().setLastImportPath(QFileInfo(filePath).absolutePath());

    const auto& texture = (*mTexturesPtr)[index];
    texture->textureData().save(filePath);
}

void TextureListWidget::setupListItem(TextureListItem* item, int index) {
    const auto& texture = (*mTexturesPtr)[index];
    QPixmap pixmap = QPixmap::fromImage(texture->textureData());
    auto format = texture->textureFormat();
    auto width = pixmap.width();
    auto height = pixmap.height();
    auto sizeBytes = texture->textureDataRaw().size();
    auto userCount = texture->userCount();

    QString sizeString = (sizeBytes < 1024) ?
        QString("%1 Bytes").arg(sizeBytes) :
        QString("%1 KB").arg(sizeBytes / 1024);

    QString text = QString("Format: %1 \nDimentions: %2x%3\nSize: %4\nUsers: %5")
        .arg(toString(format))
        .arg(width)
        .arg(height)
        .arg(sizeString)
        .arg(userCount);

    // Update existing item
    if (item) {
        QList<ThumbnailWidget*> thumbnails = item->findChildren<ThumbnailWidget*>();
        if (!thumbnails.isEmpty()) {
            thumbnails.first()->setPixmap(pixmap);
        }

        QList<QLabel*> labels = item->findChildren<QLabel*>();
        if (!labels.isEmpty()) {
            labels.first()->setText(text);
        }

        item->update();
    } else {
        // Create a new item
        auto* newItem = new TextureListItem(text, QIcon(pixmap), &mGridContainer);
        newItem->setFixedSize(200, 74);// TODO: Size dynamically
        newItem->setProperty("textureIndex", index);
        connect(newItem, &TextureListItem::exportImage, this, &TextureListWidget::exportImage);
        connect(newItem, &TextureListItem::replaceTexture, this, &TextureListWidget::replaceTexture);

        texture->setUserCountCallback([this, index](u32 userCount) {
            Q_UNUSED(userCount);
            updateItemAt(index);
        });

        mItemWidgets.push_back(newItem);
    }
}

void TextureListWidget::populateList() {
    qDeleteAll(mItemWidgets);
    mItemWidgets.clear();

    if (!mTexturesPtr) {
        return;
    }

    for (int index = 0; index < mTexturesPtr->size(); ++index) {
        setupListItem(nullptr, index);
    }

    relayoutGrid();
}

void TextureListWidget::updateItemAt(int index) {
    if (!mTexturesPtr || index < 0 || index >= mTexturesPtr->size() || index >= mItemWidgets.size()) {
        return;
    }

    setupListItem(mItemWidgets[index], index);
}

void TextureListWidget::relayoutGrid() {
    const int cardWidth = 200;
    const int spacing = mGridLayout.spacing();
    const int availableWidth = mScrollArea.viewport()->width();
    const int columns = std::max(1, availableWidth / (cardWidth + spacing));

    if (columns == mLastColumnCount && mItemWidgets.size() == mLastWidgetCount) {
        return;
    }

    mLastColumnCount = columns;
    mLastWidgetCount = mItemWidgets.size();

    while (QLayoutItem* child = mGridLayout.takeAt(0)) {
        mGridLayout.removeItem(child);
        delete child;
    }

    for (int i = 0; i < mItemWidgets.size(); ++i) {
        int row = i / columns;
        int col = i % columns;
        mGridLayout.addWidget(mItemWidgets[i], row, col);
    }
}

void TextureListWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    relayoutGrid();
}


// ========================================================================== //


} // namespace PtclEditor
