#pragma once

#include "ptcl/ptclFieldData.h"
#include "util/bitflagUtil.h"

#include <QVBoxLayout>
#include <QWidget>

namespace PtclEditor {


// ========================================================================== //


class FieldEditorWidget final : public QWidget {
    Q_OBJECT
public:
    explicit FieldEditorWidget(QWidget* parent = nullptr);

    void setData(Ptcl::FieldData* fieldData, const BitFlag<Ptcl::FieldFlag>& fieldFlag);

signals:
    void flagsUpdated(const BitFlag<Ptcl::FieldFlag>& fieldFlag);

private:
    void setupLayout(QVBoxLayout* mainLayout);
    void setupConnections();
    void setCombinerPropertiesSrc();

private:
    class RandomDataWidget;
    class MagnetDataWidget;
    class SpinDataWidget;
    class CollisionDataWidget;
    class ConvergenceDataWidget;
    class PosAddDataWidget;

private:
    Ptcl::FieldData* mDataPtr{nullptr};
    BitFlag<Ptcl::FieldFlag> mFieldFlag{};

    QWidget* mSectionsContainer{};
    RandomDataWidget* mRandomDataWidget{nullptr};
    MagnetDataWidget* mMagnetDataWidget{nullptr};
    SpinDataWidget* mSpinDataWidget{nullptr};
    CollisionDataWidget* mCollisionDataWidget{nullptr};
    ConvergenceDataWidget* mConvergenceDataWidget{nullptr};
    PosAddDataWidget* mPosAddDataWidget{nullptr};
};


// ========================================================================== //


} // namespace PtclEditor
