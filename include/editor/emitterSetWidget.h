#pragma once

#include "components/sizedSpinBox.h"
#include "ptcl/ptclEmitterSet.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterSetWidget final : public QWidget {
    Q_OBJECT
public:
    explicit EmitterSetWidget(QWidget* parent = nullptr);

    void setEmitterSet(Ptcl::EmitterSet* emitterSet);
    void clear();

signals:
    void emitterSetNamedChanged();
    void propertiesChanged();

private:
    void populateProperties();
    void setupConnections();

private:
    Ptcl::EmitterSet* mEmitterSetPtr{nullptr};
    s32 mCurEmitterIdx{};

    QLineEdit mNameLineEdit{};
    SizedSpinBox<u32> mUserDataSpinBox{};
    SizedSpinBox<u32> mLastUpdateSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
