#pragma once

#include "ptcl/ptclEnum.h"
#include "typedefs.h"

#include <QSet>
#include <QSortFilterProxyModel>


namespace PtclEditor {


// ========================================================================== //


class TextureFilterProxyModel final : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit TextureFilterProxyModel(QObject* parent = nullptr);

    void setEnabledFormats(const QSet<Ptcl::TextureFormat>& formats);
    void setShowUnusedOnly(bool show);
    void setMaxSize(s32 maxSize);
    void setMaxFileSize(s64 maxFileSize);

    void refreshFilter();

protected:
    bool filterAcceptsRow(s32 sourceRow, const QModelIndex& sourceParent) const override;

private:
    QSet<Ptcl::TextureFormat> mEnabledFormats{};
    bool mShowUnusedOnly{false};
    s32 mMaxSize{0};
    s64 mMaxFileSize{0};
};


// ========================================================================== //


} // namespace PtclEditor
