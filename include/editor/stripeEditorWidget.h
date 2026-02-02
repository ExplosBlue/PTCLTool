#pragma once

#include "editor/components/vectorSpinBox.h"
#include "ptcl/ptclEnum.h"
#include "ptcl/ptclStripeData.h"
#include "util/bitflagUtil.h"

#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class StripeEditorWidget final : public QWidget {
    Q_OBJECT
public:
    explicit StripeEditorWidget(QWidget* parent = nullptr);

    void setData(const Ptcl::StripeData& data, const BitFlag<Ptcl::StripeFlag>& stripeFlag);

signals:
    void dataUpdated(const Ptcl::StripeData& data);
    void flagsUpdated(const BitFlag<Ptcl::StripeFlag>& stripeFlags);

private:
    void setupConnections();

private:
    Ptcl::StripeData mData{};
    BitFlag<Ptcl::StripeFlag> mStripeFlag{};

    QComboBox mTypeComboBox{};
    QSpinBox mNumHistSpinBox{};
    QDoubleSpinBox mStartAlphaSpinBox{};
    QDoubleSpinBox mEndAlphaSpinBox{};
    VectorSpinBox<Math::Vector2f> mUVScrollSpeedSpinBox{};
    QSpinBox mHistStepSpinBox{};
    QDoubleSpinBox mDirIntepolateSpinBox{};
    QCheckBox mEmitterCoordCheckBox{};
};

// ========================================================================== //


} // namespace PtclEditor
