#include "editor/textureListWidget.h"

#include <QStandardItemModel>

#include "ptcl/ptcl.h"

#include "editor/thumbnailWidget.h"

namespace PtclEditor {

TextureListItem::TextureListItem(const QString& text, QIcon thumbnail, QWidget* parent) :
    QWidget{parent},
    mThumbnail{std::move(thumbnail)} {

    // Create a layout for the custom item
    auto* layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignTop);
    layout->setContentsMargins(5, 5, 5, 5);

    // Create a label for the thumbnail
    ThumbnailWidget* thumbnailWidget = new ThumbnailWidget(this);
    thumbnailWidget->setPixmap(mThumbnail.pixmap(64, 64));
    thumbnailWidget->setThumbnailSize(QSize(64, 64));
    layout->addWidget(thumbnailWidget);

    // Create a label for the text
    QLabel* textLabel = new QLabel(text, this);
    textLabel->setWordWrap(true);
    layout->addWidget(textLabel);

    // Create a button that only shows on hover (replace/remove)
    mReplaceButton = new QPushButton(QIcon(":/res/icons/replace.png"), "", this);
    mRemoveButton = new QPushButton(QIcon(":/res/icons/remove.png"), "", this);

    mRemoveButton->move(geometry().topLeft());
    mRemoveButton->setFixedSize(16, 16);
    mRemoveButton->setFlat(true);

    mReplaceButton->setVisible(false);  // Hidden by default
    mRemoveButton->setVisible(false);   // Hidden by default

    setLayout(layout);
}

void TextureListItem::enterEvent(QEnterEvent* event) {
    mRemoveButton->setVisible(true);
    QWidget::enterEvent(event);
}

void TextureListItem::leaveEvent(QEvent* event) {
    mRemoveButton->setVisible(false);
    QWidget::leaveEvent(event);
}


// ========================================================================== //


TextureListWidget::TextureListWidget(QWidget *parent) :
    QWidget{parent},
    mTexturesPtr{nullptr},
    mScrollArea{this} {

    QVBoxLayout* mainLayout = new QVBoxLayout;

    // Grid Container
    mGridLayout.setSpacing(10);
    mGridLayout.setContentsMargins(10, 10, 10, 10);
    mGridContainer.setLayout(&mGridLayout);

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

void TextureListWidget::populateList() {
    qDeleteAll(mItemWidgets);
    mItemWidgets.clear();

    if (!mTexturesPtr) {
        return;
    }

    for (auto& texture : *mTexturesPtr) {
        QPixmap pixmap = QPixmap::fromImage(texture->textureData());
        auto format = texture->textureFormat();

        auto width = pixmap.width();
        auto height = pixmap.height();
        auto userCount = texture->userCount();

        auto text = QString("Format: %1 \nDimentions: %2x%3\n\nUsers: %4")
            .arg(toString(format))
            .arg(width)
            .arg(height)
            .arg(userCount);

        auto* textureItem = new TextureListItem(text, QIcon(pixmap), &mGridContainer);
        textureItem->setFixedSize(200, 120);
        mItemWidgets.push_back(textureItem);
    }
    relayoutGrid();
}

void TextureListWidget::relayoutGrid() {
    const int cardWidth = 200;
    const int spacing = mGridLayout.spacing();
    const int availableWidth = mScrollArea.viewport()->width();
    const int columns = std::max(1, availableWidth / (cardWidth + spacing));

    QLayoutItem* child;
    while ((child = mGridLayout.takeAt(0)) != nullptr) {
        if (QWidget* widget = child->widget()) {
            mGridLayout.removeWidget(widget);
        }
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

} // namespace PtclEditor
