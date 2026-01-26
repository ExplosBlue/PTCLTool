#pragma once

#include "ptcl/ptclChildData.h"

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QStackedWidget>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class ChildEditorWidget final : public QWidget {
    Q_OBJECT
public:
    explicit ChildEditorWidget(QWidget* parent = nullptr);

    void setChildData(Ptcl::ChildData* childData);
    // void setTextureList(const Ptcl::TextureList* textureList);

    void clear();

private:
    void setupLayout(QVBoxLayout* mainLayout);
    void setupConnections();

private:
    class BasicPropertiesWidget;

private:
    Ptcl::ChildData* mDataPtr{nullptr};
    // const Ptcl::TextureList* mTextureList{nullptr};

    BasicPropertiesWidget* mBasicProperties{nullptr};
};


// ========================================================================== //


} // namespace PtclEditor
