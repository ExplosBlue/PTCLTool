#pragma once

#include "editor/components/stickyMenu.h"
#include "ptcl/ptclEnum.h"
#include "typedefs.h"

#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QSet>


namespace PtclEditor {


// ========================================================================== //


struct TextureFilterState {
    QSet<Ptcl::TextureFormat> formats;
    bool unusedOnly{};
    s32 maxDimension{};
    qint64 maxFileSize{};
};


// ========================================================================== //


class TextureFilterPopup final : public StickyMenu {
    Q_OBJECT
public:
    explicit TextureFilterPopup(QWidget* parent = nullptr);

    void reset();
    void popup(const QPoint& anchorGlobal);

signals:
    void filterChanged(const PtclEditor::TextureFilterState& state);
    void closed();

protected:
    void hideEvent(QHideEvent* event) override;

private:
    void createFormatMenu();
    void createActions();
    void createSizeMenu();
    void createFileSizeMenu();

    void updateFormatFilter();
    void updateSizeMenuLabel();
    void updateFileSizeMenuLabel();

private:
    StickyMenu* mFormatMenu{nullptr};
    StickyMenu* mSizeMenu{nullptr};
    StickyMenu* mFileSizeMenu{nullptr};

    QActionGroup mFormatGroup{nullptr};
    QActionGroup mSizeGroup{nullptr};
    QActionGroup mFileSizeGroup{nullptr};

    QAction mUnusedOnlyAction{};
    TextureFilterState mState{};
};


// ========================================================================== //


} // namespace PtclEditor
