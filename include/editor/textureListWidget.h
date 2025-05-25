#ifndef TEXTURELISTWIDGET_H
#define TEXTURELISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QHBoxLayout>
#include <QPushButton>

#include <QLabel>
#include <QStyledItemDelegate>
#include <QPainter>


#include <QtMinMax>

#include "ptcl/ptcl.h"


namespace PtclEditor {


class TextureListItem final : public QWidget
{
    Q_OBJECT

public:
    TextureListItem(const QString& text, const QIcon& thumbnail, QWidget* parent = nullptr)
        : QWidget(parent), mThumbnail(thumbnail) {

        // Create a layout for the custom item
        QHBoxLayout* layout = new QHBoxLayout(this);

        // Create a label for the thumbnail
        QLabel* thumbnailLabel = new QLabel(this);
        thumbnailLabel->setPixmap(mThumbnail.pixmap(64, 64));  // Set size of thumbnail
        layout->addWidget(thumbnailLabel);

        // Create a label for the text
        QLabel* textLabel = new QLabel(text, this);
        layout->addWidget(textLabel);

        // Create a button that only shows on hover (replace/remove)
        mReplaceButton = new QPushButton(QIcon(":/res/icons/replace.png"), "", this);
        mRemoveButton = new QPushButton(QIcon(":/res/icons/remove.png"), "", this);


        // mReplaceButton->move(geometry().topRight());
        // I don't like this being hardcoded... (use font relative size maybe?)
        mRemoveButton->move(geometry().topLeft());
        mRemoveButton->setFixedSize(16, 16);
        mRemoveButton->setFlat(true);

        // mRemoveButton->setGeometry(QRect{QPoint{rect().top(), rect().right() + mReplaceButton->width()}, mReplaceButton->size()});

        mReplaceButton->setVisible(false);  // Hidden by default
        mRemoveButton->setVisible(false);   // Hidden by default

        // layout->addWidget(mReplaceButton);
        // layout->addWidget(mRemoveButton);

        this->setLayout(layout);
    }

    // TODO: Enter/Leave events => hide/show buttons
protected:
    void enterEvent(QEnterEvent* event) final {
        mRemoveButton->setVisible(true);
        // mReplaceButton->setVisible(true);
    }

    void leaveEvent(QEvent* event) final {
        mRemoveButton->setVisible(false);
        // mReplaceButton->setVisible(false);
    }


private:
    QIcon mThumbnail;
    QPushButton* mReplaceButton;
    QPushButton* mRemoveButton;
};


class TextureListWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TextureListWidget(QWidget* parent = nullptr);

    void setTextures(Ptcl::TextureList* textures);

private:
    void populateList();

private:
    // This should probably be a weak ptr?
    Ptcl::TextureList* mTexturesPtr;

    QListWidget mListWidget;

    QHBoxLayout mMainLayout;

    // TODO: Texture Info (move to dedicated widget?)
    // QLabel mImageLabel;
    // QLabel mImageInfoLabel;

};

} // namespace PtclEditor

#endif // TEXTURELISTWIDGET_H
