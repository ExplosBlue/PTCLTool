#pragma once

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
    QSpinBox mUserDataSpinBox{};
    QSpinBox mLastUpdateSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
