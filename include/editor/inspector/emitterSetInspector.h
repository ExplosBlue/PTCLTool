#pragma once

#include "editor/inspector/inspectorWidgetBase.h"

#include <QLineEdit>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterSetInspector final : public InspectorWidgetBase {
    Q_OBJECT
public:
    explicit EmitterSetInspector(QWidget* parent = nullptr);

    void setDocument(Ptcl::Document* document) override;
    void setSelection(Ptcl::Selection* selection) override;

private:
    void populateProperties() final;
    void setupConnections();

private:
    const Ptcl::EmitterSet* mEmitterSet{nullptr};

    QLineEdit mNameLineEdit{};
};


// ========================================================================== //


} // namespace PtclEditor
