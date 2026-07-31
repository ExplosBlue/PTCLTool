#pragma once

#include "editor/components/stickyMenu.h"
#include "ptcl/ptclEnum.h"
#include "typedefs.h"

#include <QActionGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QEvent>
#include <QFrame>
#include <QPushButton>
#include <QSet>
#include <QToolButton>


namespace PtclEditor {


// ========================================================================== //


class TextureFilterBar final : public QFrame {
    Q_OBJECT
public:
    explicit TextureFilterBar(QWidget* parent = nullptr);

    void reset();

signals:
    void formatsChanged(const QSet<Ptcl::TextureFormat>& formats);
    void unusedOnlyChanged(bool enabled);
    void sizeChanged(s32 maxSize);
    void fileSizeChanged(s32 maxFileSize);

private:
    void setupStyle();
    void setupFormatButton();
    void setupUnusedOnlyCheck();
    void setupSizeCombo();
    void setupMaxFileSizeCombo();
    void setupClearButton();
    void setupLayout();
    void setupFormatMenu();
    void updateFormatFilter();
    void rebuildStyle();

protected:
    void changeEvent(QEvent* event) override;

private:
    StickyMenu mFormatMenu{};
    QActionGroup mFormatGroup{nullptr};
    QToolButton mFormatButton{};
    QCheckBox mUnusedOnlyCheck{};
    QComboBox mSizeCombo{};
    QComboBox mMaxFileSizeCombo{};
    QPushButton mClearFiltersButton{};
    bool mUpdatingStyle{false};
};


// ========================================================================== //


} // namespace PtclEditor
