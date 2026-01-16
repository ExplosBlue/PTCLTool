#include "editor/collapsibleWidget.h"
#include "editor/emitterWidget.h"
#include "util/nameValidator.h"

#include <QScrollArea>


namespace PtclEditor {


// ========================================================================== //


EmitterWidget::EmitterWidget(QWidget* parent) :
    QWidget{parent}, mIsPopulating{false} {
    auto* containerWidget = new QWidget;
    containerWidget->setLayout(&mMainLayout);

    containerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    containerWidget->setMinimumWidth(400);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(containerWidget);
    scrollArea->setWidgetResizable(true);

    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->addWidget(scrollArea);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(outerLayout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mMainLayout.setColumnStretch(0, 0);
    mMainLayout.setColumnStretch(1, 1);
    mMainLayout.setColumnStretch(2, 0);
    mMainLayout.setColumnStretch(3, 1);

    auto addLabledWidget = [&](QWidget* widget, const QString& label, int row, int column, int span = 1) {
        mMainLayout.addWidget(new QLabel(label), row, column);
        mMainLayout.addWidget(widget,            row, column + 1, 1, span);
    };


    // Basic Properties
    connect(&mBasicProperties, &BasicPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::BasicProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setBasicProperties(properties);
    });

    auto basicPropertiesSection = new CollapsibleWidget("Basic properties", this);
    basicPropertiesSection->setContent(&mBasicProperties);
    mMainLayout.addWidget(basicPropertiesSection, 0, 0, 1, 4);


    // Texture Properties
    auto texPropertiesSection = new CollapsibleWidget("Texture properties", this);
    texPropertiesSection->setContent(&mTextureProperties);

    mMainLayout.addWidget(texPropertiesSection, 5, 0, 1, 4);
    connect(&mTextureProperties, &TexturePropertiesWidget::textureUpdated, this, [=, this](int oldIndex, int index) {
        emit textureUpdated(oldIndex, index);
    });

    addLabledWidget(&m_2CSpinBox, "_2C:",  6, 0, 3);
    addLabledWidget(&m_2DSpinBox, "_2D:",  7, 0, 3);
    addLabledWidget(&m_2ESpinBox, "_2E:",  8, 0, 3);
    addLabledWidget(&m_30SpinBox, "_30:",  9, 0, 3);
    addLabledWidget(&m_31SpinBox, "_31:", 10, 0, 3);

    // Gravity Properties
    connect(&mGravityProperties, &GravityPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::GravityProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setGravityProperties(properties);
    });

    auto gravityPropertiesSection = new CollapsibleWidget("Gravity properties", this);
    gravityPropertiesSection->setContent(&mGravityProperties);
    mMainLayout.addWidget(gravityPropertiesSection, 11, 0, 1, 4);

    // Lifespan Properties
    connect(&mLifespanProperties, &LifespanPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::LifespanProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setLifespanProperties(properties);
    });

    auto lifespanPropertiesSection = new CollapsibleWidget("Lifespan properties", this);
    lifespanPropertiesSection->setContent(&mLifespanProperties);
    mMainLayout.addWidget(lifespanPropertiesSection, 12, 0, 1, 4);

    // Termination Properties
    connect(&mTerminationProperties, &TerminationPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::TerminationProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setTerminationProperties(properties);
    });

    auto terminationPropertiesSection = new CollapsibleWidget("Termination properties", this);
    terminationPropertiesSection->setContent(&mTerminationProperties);
    mMainLayout.addWidget(terminationPropertiesSection, 13, 0, 1, 4);

    // Emission Properties
    connect(&mEmissionProperties, &EmissionPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::EmissionProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setEmissionProperties(properties);
    });

    auto emissionPropertiesSection = new CollapsibleWidget("Emission properties", this);
    emissionPropertiesSection->setContent(&mEmissionProperties);
    mMainLayout.addWidget(emissionPropertiesSection, 14, 0, 1, 4);

    // Volume Properties
    connect(&mVolumeProperties, &VolumePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::VolumeProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setVolumeProperties(properties);
    });

    auto volumePropertiesSection = new CollapsibleWidget("Volume properties", this);
    volumePropertiesSection->setContent(&mVolumeProperties);
    mMainLayout.addWidget(volumePropertiesSection, 15, 0, 1, 4);

    // Velocity Properties
    connect(&mVelocityProperties, &VelocityPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::VelocityProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setVelocityProperties(properties);
    });

    auto velocityPropertiesSection = new CollapsibleWidget("Velocity properties", this);
    velocityPropertiesSection->setContent(&mVelocityProperties);
    mMainLayout.addWidget(velocityPropertiesSection, 16, 0, 1, 4);

    // Transform Properties
    connect(&mTransformProperties, &TransformPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::TransformProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setTransformProperties(properties);
    });

    auto transformPropertiesSection = new CollapsibleWidget("Transform properties", this);
    transformPropertiesSection->setContent(&mTransformProperties);
    mMainLayout.addWidget(transformPropertiesSection, 17, 0, 1, 4);

    // _70

    m_8CSpinBox.setMaximum(std::numeric_limits<f32>::max());
    m_8CSpinBox.setMinimum(std::numeric_limits<f32>::min());
    addLabledWidget(&m_8CSpinBox, "_8C:", 26, 0, 3);

    addLabledWidget(&m_90SpinBox,        "_90:",            27, 0, 3);
    addLabledWidget(&mBillboardComboBox, "Billboard Type:", 28, 0, 3);
    addLabledWidget(&m_98SpinBox,        "_98:",            29, 0, 3);

    // Color Properties
    connect(&mColorProperties, &ColorPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ColorProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setColorProperties(properties);
    });

    auto colorPropertiesSection = new CollapsibleWidget("Color properties", this);
    colorPropertiesSection->setContent(&mColorProperties);
    mMainLayout.addWidget(colorPropertiesSection, 33, 0, 1, 4);

    // _D8

    // Rotation Properties
    connect(&mAlphaProperties, &AlphaPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::AlphaProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setAlphaProperties(properties);
    });

    auto alphaPropertiesSection = new CollapsibleWidget("Alpha properties", this);
    alphaPropertiesSection->setContent(&mAlphaProperties);
    mMainLayout.addWidget(alphaPropertiesSection, 34, 0, 1, 4);

    // Rotation Properties
    connect(&mRotationProperties, &RotationPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::RotationProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setRotationProperties(properties);
    });

    auto rotationPropertiesSection = new CollapsibleWidget("Rotation properties", this);
    rotationPropertiesSection->setContent(&mRotationProperties);
    mMainLayout.addWidget(rotationPropertiesSection, 49, 0, 1, 4);

    addLabledWidget(&m_134SpinBox, "_134:", 52, 0, 3);

    // Scale Properties
    connect(&mScaleProperties, &ScalePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ScaleProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setScaleProperties(properties);
    });

    auto scalePropertiesSection = new CollapsibleWidget("Scale properties", this);
    scalePropertiesSection->setContent(&mScaleProperties);
    mMainLayout.addWidget(scalePropertiesSection, 53, 0, 1, 4);
}

void EmitterWidget::setEmitter(Ptcl::Emitter* emitter) {
    mIsPopulating = true;

    mEmitterPtr = emitter;

    mTextureProperties.setEmitter(mEmitterPtr);

    // More Stuff
    m_2CSpinBox.setValue(mEmitterPtr->_2C());
    m_2DSpinBox.setValue(mEmitterPtr->_2D());
    m_2ESpinBox.setValue(mEmitterPtr->_2E());
    m_30SpinBox.setValue(mEmitterPtr->_30());
    m_31SpinBox.setValue(mEmitterPtr->_31());

    m_8CSpinBox.setValue(mEmitterPtr->_8C());
    m_90SpinBox.setValue(mEmitterPtr->_90());
    mBillboardComboBox.setCurrentEnum((mEmitterPtr->billboardType()));

    m_134SpinBox.setValue(mEmitterPtr->_134());

    mBasicProperties.setProperties(mEmitterPtr->basicProperties());
    mGravityProperties.setProperties(mEmitterPtr->gravityProperties());
    mTransformProperties.setProperties(mEmitterPtr->transformProperties());
    mLifespanProperties.setProperties(mEmitterPtr->lifespanProperties());
    mTerminationProperties.setProperties(mEmitterPtr->terminationProperties());
    mEmissionProperties.setProperties(mEmitterPtr->emissionProperties());
    mVelocityProperties.setProperties(mEmitterPtr->velocityProperties());
    mVolumeProperties.setProperties(mEmitterPtr->volumeProperties());
    mColorProperties.setProperties(mEmitterPtr->colorProperties());
    mAlphaProperties.setProperties(mEmitterPtr->alphaProperties());
    mScaleProperties.setProperties(mEmitterPtr->scaleProperties());
    mRotationProperties.setProperties(mEmitterPtr->rotationProperties());

    mIsPopulating = false;
}

TexturePropertiesWidget& EmitterWidget::getTextureProperties() {
    return mTextureProperties;
}


// ========================================================================== //


} // namespace PtclEditor
