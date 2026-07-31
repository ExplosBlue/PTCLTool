#pragma once

#include "editor/components/thumbnailWidget.h"
#include "ptcl/ptclTexture.h"

#include <QLabel>
#include <QModelIndex>
#include <QPushButton>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class TextureDetailsPanel final : public QWidget {
    Q_OBJECT
public:
    explicit TextureDetailsPanel(QWidget* parent = nullptr);

    void setTexture(const QModelIndex& index, Ptcl::Texture* texture);
    void refreshTexture();
    bool matchesIndex(const QModelIndex& index) const { return mIndex == index && mIndex.isValid(); }

signals:
    void exportRequested(Ptcl::Texture* texture);
    void replaceRequested(const QModelIndex& index);
    void deleteRequested(const QModelIndex& index);

private:
    void updateLabels();

private:
    Ptcl::Texture* mTexturePtr{nullptr};
    QModelIndex mIndex;

    ThumbnailWidget mThumbnailWidget{};
    QLabel mNameLabel{};
    QLabel mFormatLabel{};
    QLabel mDimensionsLabel{};
    QLabel mSizeLabel{};
    QLabel mUsersLabel{};
    QPushButton mExportButton{};
    QPushButton mReplaceButton{};
    QPushButton mDeleteButton{};
};


// ========================================================================== //


} // namespace PtclEditor
