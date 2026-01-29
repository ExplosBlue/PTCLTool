#pragma once

#include "ptcl/ptclEnum.h"
#include "ptcl/ptclFluctuationData.h"
#include "util/bitflagUtil.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class FluctuationEditorWidget final : public QWidget {
    Q_OBJECT
public:
    explicit FluctuationEditorWidget(QWidget* parent = nullptr);

    void setData(const Ptcl::FluctuationData& data, const BitFlag<Ptcl::FluctuationFlag>& fluxFlag);

signals:
    void dataUpdated(const Ptcl::FluctuationData& data);
    void flagsUpdated(const BitFlag<Ptcl::FluctuationFlag>& fluxFlags);

private:
    void setupConnections();

private:
    Ptcl::FluctuationData mData{};
    BitFlag<Ptcl::FluctuationFlag> mFluxFlag{};

    QWidget* mControlsContainer{nullptr};
    QDoubleSpinBox mScaleSpinBox{};
    QDoubleSpinBox mFreqSpinBox{};
    QSpinBox mPhaseRndSpinBox{};
    QCheckBox mApplyAlphaCheckBox{};
    QCheckBox mApplyScaleCheckBox{};
    QCheckBox mEnabledCheckBox{};
};

// ========================================================================== //


} // namespace PtclEditor
