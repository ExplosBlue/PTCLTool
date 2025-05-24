#include "editor/textureWidget.h"

#include <QStandardItemModel>

#include "ptcl/ptcl.h"

namespace PtclEditor {

TextureWidget::TextureWidget(QWidget *parent) :
    mTexturesPtr{nullptr}, QWidget{parent} {

    setLayout(&mMainLayout);

    mListWidget.setFlow(QListView::LeftToRight);
    mListWidget.setWrapping(true);
    mListWidget.setLayoutMode(QListView::SinglePass);
    mListWidget.setVerticalScrollMode(QListView::ScrollPerPixel);
    mListWidget.setMovement(QListView::Static);

    mListWidget.setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mListWidget.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    mMainLayout.addWidget(&mListWidget);
}

void TextureWidget::setTextures(Ptcl::TextureList* textures) {

    if (mTexturesPtr == textures) {
        return;
    }

    mListWidget.clear();

    mTexturesPtr = textures;

    populateList();
}

void TextureWidget::populateList() {

    if (!mTexturesPtr) {
        return;
    }

    mListWidget.clear();

    for (auto& texture : *mTexturesPtr) {

        QImage img = texture->textureData();
        auto format = texture->textureFormat();

        auto width = img.width();
        auto height = img.height();
        auto userCount = texture->userCount();

        QPixmap pixmap = QPixmap::fromImage(img);

        QListWidgetItem* item = new QListWidgetItem(&mListWidget);

        TextureListItem* textureItem = new TextureListItem(
            QString("Format: %1 \nDimentions: %2x%3\n\nUsers: %4").arg(toString(format)).arg(width).arg(height).arg(userCount),
            QIcon(pixmap.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation))
            );

        item->setSizeHint(textureItem->sizeHint());
        mListWidget.setItemWidget(item, textureItem);

        // TODO: this should only occur one time (during initialization?)
        // Move/handle this in a better way
        mListWidget.setMinimumHeight(item->sizeHint().height() * 2);
    }
}

} // namespace PtclEditor
