#pragma once

#include "editor/components/sizedSpinBox.h"
#include "ptcl/ptclDocument.h"
#include "ptcl/ptclEmitterSet.h"

#include <QLineEdit>
#include <QSpinBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterSetInspector final : public QWidget {
    Q_OBJECT
public:
    explicit EmitterSetInspector(QWidget* parent = nullptr);

    void setDocument(Ptcl::Document* document);
    void setSelection(Ptcl::Selection* selection);

private:
    void populateProperties();
    void setupConnections();

private:
    Ptcl::Document* mDocument{nullptr};
    const Ptcl::Selection* mSelection{nullptr};
    const Ptcl::EmitterSet* mEmitterSet{nullptr};

    QLineEdit mNameLineEdit{};
    SizedSpinBox<u32> mUserDataSpinBox{};
    SizedSpinBox<u32> mLastUpdateSpinBox{};
};


// ========================================================================== //


} // namespace PtclEditor
