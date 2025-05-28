#include "editor/textureSelectDialog.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>

TextureSelectDialog::TextureSelectDialog(const Ptcl::TextureList& textures, QWidget* parent) :
    QDialog{parent},
    mSelectedIndex{-1} {

    // List Widget
    mListWidget.setIconSize(QSize(64, 64));
    mListWidget.setUniformItemSizes(true);
    for (size_t i = 0; i < textures.size(); ++i) {
        const auto& texture = textures[i];
        QListWidgetItem* item = new QListWidgetItem(QString("Texture %1").arg(i));
        QPixmap thumbnail = QPixmap::fromImage(textures[i]->textureData());
        item->setIcon(QIcon(thumbnail.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
        mListWidget.addItem(item);
    }

    // Dialog Buttons
    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttons, &QDialogButtonBox::accepted, this, [=, this]() {
        if (mListWidget.currentRow() >= 0) {
            mSelectedIndex = mListWidget.currentRow();
            accept();
        }
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(&mListWidget);
    mainLayout->addWidget(buttons);
}

int TextureSelectDialog::selectedIndex() const  {
    return mSelectedIndex;
}
