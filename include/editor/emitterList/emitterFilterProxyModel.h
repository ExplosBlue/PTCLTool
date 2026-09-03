#pragma once

#include "editor/emitterList/emitterListTypes.h"

#include <QSortFilterProxyModel>


namespace PtclTool {


// ========================================================================== //


class EmitterFilterProxyModel : public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit EmitterFilterProxyModel(QObject* parent = nullptr);

    void setEmitterFilter(const EmitterFilter& filter);

protected:
    bool filterAcceptsRow(s32 sourceRow, const QModelIndex& sourceParent) const override;
    QVariant data(const QModelIndex& index, s32 role) const override;

private:
    EmitterFilter mEmitterFilter{EmitterFilterFlag::Simple, EmitterFilterFlag::Complex, EmitterFilterFlag::Compact};
};


// ========================================================================== //


} //namespace PtclTool
