#pragma once

#include "editor/components/thumbnailWidget.h"
#include "ptcl/ptclTexture.h"
#include "ptcl/ptcl.h"

#include <QLabel>
#include <QModelIndex>
#include <QPushButton>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QWidget>


namespace Ptcl {
class Document;
class Selection;
}


namespace PtclEditor {


// ========================================================================== //


class TextureDetailsPanel final : public QWidget {
    Q_OBJECT
public:
    explicit TextureDetailsPanel(QWidget* parent = nullptr);

    void setDocument(Ptcl::Document* document);
    void setSelection(Ptcl::Selection* selection);

    void setTexture(const QModelIndex& index, Ptcl::Texture* texture);
    void refresh();
    void refreshUsers();
    bool matchesIndex(const QModelIndex& index) const { return mIndex == index && mIndex.isValid(); }

signals:
    void exportRequested(Ptcl::Texture* texture);
    void replaceRequested(const QModelIndex& index);
    void deleteRequested(const QModelIndex& index);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void updateLabels();
    void updateUsers();
    void navigateToUser(QTreeWidgetItem* item);

    void setupDetailsPage();
    void setupUsersPage();
    void setupTabs();
    void setupConnections();

    void clearDetails();

    void addUsageItem(const Ptcl::TextureUsage& usage);

    void showEmptyUsers();
    void setUsersCount(s32 count);

private:
    enum ItemRole {
        SetIndexRole = Qt::UserRole,
        EmitterIndexRole,
        ChildRole,
    };

private:
    Ptcl::Document* mDocument{nullptr};
    Ptcl::Selection* mSelection{nullptr};

    Ptcl::Texture* mTexturePtr{nullptr};
    QModelIndex mIndex;

    QTabWidget mTabs{};
    QWidget mDetailsPage{};
    QWidget mUsersPage{};

    ThumbnailWidget mThumbnailWidget{};
    QLabel mNameLabel{};
    QLabel mFormatLabel{};
    QLabel mDimensionsLabel{};
    QLabel mSizeLabel{};

    QLabel mUsersHeaderLabel{};
    QStackedWidget mUsersStack{};
    QLabel mUsersEmptyLabel{};
    QTreeWidget mUsersList{};

    QPushButton mExportButton{};
    QPushButton mReplaceButton{};
    QPushButton mDeleteButton{};
};


// ========================================================================== //


} // namespace PtclEditor
