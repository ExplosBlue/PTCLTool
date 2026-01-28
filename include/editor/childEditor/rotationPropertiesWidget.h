#pragma once

#include "editor/components/enumComboBox.h"
#include "editor/components/vectorSpinBox.h"
#include "editor/childEditor/childEditorWidget.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget::RotationPropertiesWidget final : public QWidget {
    Q_OBJECT
public:
    explicit RotationPropertiesWidget(QWidget* parent = nullptr);

    void setProperties(const Ptcl::ChildData::RotationProperties& properties, bool inheritRotation);

signals:
    void propertiesUpdated(const Ptcl::ChildData::RotationProperties& properties);
    void inheritRotationUpdated(bool inherit);

private:
    void setupConnections();
    void updateAxis();

private:
    Ptcl::ChildData::RotationProperties mProps{};

    EnumComboBox<Ptcl::RotType> mRotTypeSpinBox{};
    VectorSpinBox<Math::Vector3f> mInitRotSpinBox{};
    VectorSpinBox<Math::Vector3f> mInitRotRandSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotVelSpinBox{};
    VectorSpinBox<Math::Vector3f> mRotVelRandSpinBox{};
    VectorSpinBox<Math::Vector2f> mRotBasisSpinBox{};
    QCheckBox mInheritRotCheckBox{};
};


// ========================================================================== //


} // namespace PtclEditor
