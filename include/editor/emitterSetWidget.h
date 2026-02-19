#pragma once

#include "components/sizedSpinBox.h"
#include "ptcl/ptclDocument.h"
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

    void setDocument(Ptcl::Document* document);
    void setSelection(Ptcl::Selection* selection);

signals:
    void emitterSetNamedChanged();
    void propertiesChanged();

private:
    void populateProperties();
    void setupConnections();

private:
    Ptcl::Document* mDocument{nullptr};
    const Ptcl::Selection* mSelection{nullptr};
    Ptcl::EmitterSet* mEmitterSet{nullptr};

    QLineEdit mNameLineEdit{};
    SizedSpinBox<u32> mUserDataSpinBox{};
    SizedSpinBox<u32> mLastUpdateSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
