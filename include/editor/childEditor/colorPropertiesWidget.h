#pragma once

#include "editor/components/rgbaColorWidget.h"
#include "editor/childEditor/childEditorWidget.h"

#include <QCheckBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget::ColorPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ColorPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ChildData::ColorProperties& properties, bool inheritColor);

signals:
    void propertiesUpdated(const Ptcl::ChildData::ColorProperties& properties);
    void inheritColorUpdated(bool inherit);

private:
    void setupConnections();

private:
    Ptcl::ChildData::ColorProperties mProps{};

    RGBAColorWidget mColor0Widget{};
    RGBAColorWidget mColor1Widget{};
    QCheckBox mInheritColorCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
