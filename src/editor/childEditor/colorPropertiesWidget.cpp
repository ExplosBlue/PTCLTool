#include "editor/childEditor/colorPropertiesWidget.h"

#include <QFormLayout>


namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::ColorPropertiesWidget::ColorPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    mInheritColorCheckBox.setText("Inherit Parent Color 0");
    mColor1Widget.enableAlpha(false);

    mainLayout->addRow("Color:", &mInheritColorCheckBox);
    mainLayout->addRow("Color 0:", &mColor0Widget);
    mainLayout->addRow("Color 1:", &mColor1Widget);

    setLayout(mainLayout);
    setupConnections();
}

void ChildEditorWidget::ColorPropertiesWidget::setupConnections() {
    // Inherit Color
    connect(&mInheritColorCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        QSignalBlocker b1(mColor0Widget);
        mColor0Widget.setDisabled(checked);
        emit inheritColorUpdated(checked);
    });

    // Color 0
    connect(&mColor0Widget, &RGBAColorWidget::colorChanged, this, [this]() {
        mProps.color0 = mColor0Widget.color();
        emit propertiesUpdated(mProps);
    });

    // Color 1
    connect(&mColor1Widget, &RGBAColorWidget::colorChanged, this, [this]() {
        const auto& color = mColor1Widget.color();

        Ptcl::binColor3f newColor{
            color.r * 255.0f,
            color.g * 255.0f,
            color.b * 255.0f
        };

        mProps.color1 = newColor;
        emit propertiesUpdated(mProps);
    });
}

void ChildEditorWidget::ColorPropertiesWidget::setProperties(const Ptcl::ChildData::ColorProperties& properties, bool inheritColor) {
    QSignalBlocker b1(mColor0Widget);
    QSignalBlocker b2(mColor1Widget);
    QSignalBlocker b3(mInheritColorCheckBox);

    mProps = properties;

    mColor0Widget.setColor(mProps.color0);
    mColor0Widget.setDisabled(inheritColor);

    Ptcl::binColor4f color1{
        std::clamp(mProps.color1.r / 255.0f, 0.0f, 1.0f),
        std::clamp(mProps.color1.g / 255.0f, 0.0f, 1.0f),
        std::clamp(mProps.color1.b / 255.0f, 0.0f, 1.0f),
        1.0f
    };
    mColor1Widget.setColor(color1);
    mInheritColorCheckBox.setChecked(inheritColor);
}


// ========================================================================== //


} // namespace PtclEditor
