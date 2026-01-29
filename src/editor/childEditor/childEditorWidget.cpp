#include "editor/components/collapsibleWidget.h"
#include "editor/childEditor/childEditorWidget.h"
#include "editor/childEditor/combinerPropertiesWidget.h"
#include "editor/childEditor/alphaPropertiesWidget.h"
#include "editor/childEditor/basicPropertiesWidget.h"
#include "editor/childEditor/colorPropertiesWidget.h"
#include "editor/childEditor/emissionPropertiesWidget.h"
#include "editor/childEditor/rotationPropertiesWidget.h"
#include "editor/childEditor/scalePropertiesWidget.h"
#include "editor/childEditor/texturePropertiesWidget.h"
#include "editor/childEditor/velocityPropertiesWidget.h"

#include <QScrollArea>


namespace PtclEditor {


// ========================================================================== //


ChildEditorWidget::ChildEditorWidget(QWidget* parent) :
    QWidget{parent} {

    mBasicProperties = new BasicPropertiesWidget(this);
    mEmissionProperties = new EmissionPropertiesWidget(this);
    mVelocityProperties = new VelocityPropertiesWidget(this);
    mRotationProperties = new RotationPropertiesWidget(this);
    mScaleProperties = new ScalePropertiesWidget(this);
    mTextureProperties = new TexturePropertiesWidget(this);
    mColorProperties = new ColorPropertiesWidget(this);
    mAlphaProperties = new AlphaPropertiesWidget(this);
    mCombinerProperties = new CombinerPropertiesWidget(this);

    // Standard Widget
    auto* standardWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(standardWidget);
    standardWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    standardWidget->setMinimumWidth(400);

    auto* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(standardWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    auto* outerLayout = new QVBoxLayout(this);
    outerLayout->addWidget(scrollArea);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(outerLayout);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    setupLayout(mainLayout);
    setupConnections();
}

void ChildEditorWidget::setupLayout(QVBoxLayout* mainLayout) {
    // ChildData row
    auto* childDataRow = new QHBoxLayout();
    auto* childDataLabel = new QLabel("Child Emitter:");
    mEnabledCheckbox.setText("Enabled");

    childDataRow->addWidget(childDataLabel);
    childDataRow->addWidget(&mEnabledCheckbox);
    childDataRow->addStretch();

    mainLayout->addLayout(childDataRow);

    // Section Containers
    mSectionsContainer = new QWidget(this);
    auto* sectionsLayout = new QVBoxLayout(mSectionsContainer);
    sectionsLayout->setContentsMargins(0, 0, 0, 0);

    auto addSection = [sectionsLayout](const QString& title, QWidget* widget) {
        auto* section = new CollapsibleWidget(title);
        section->setContent(widget);
        sectionsLayout->addWidget(section);
    };

    addSection("Basic properties", mBasicProperties);
    addSection("Emission properties", mEmissionProperties);
    addSection("Velocity properties", mVelocityProperties);
    addSection("Rotation properties", mRotationProperties);
    addSection("Scale properties", mScaleProperties);
    addSection("Texture Properties", mTextureProperties);
    addSection("Color Properties", mColorProperties);
    addSection("Alpha Properties", mAlphaProperties);
    addSection("Combiner Properties", mCombinerProperties);

    sectionsLayout->addStretch();

    mainLayout->addWidget(mSectionsContainer);
    mainLayout->addStretch();
}

void ChildEditorWidget::setupConnections() {
    // Enabled
    connect(&mEnabledCheckbox, &QCheckBox::clicked, this, [this](bool checked) {
        if (!mDataPtr) { return; }

        mSectionsContainer->setEnabled(checked);
        checked ? mChildFlag.set(Ptcl::ChildFlag::Enabled) : mChildFlag.clear(Ptcl::ChildFlag::Enabled);
        emit flagsUpdated(mChildFlag);
    });

    // Basic Properties
    connect(mBasicProperties, &BasicPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::BasicProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setBasicProperties(properties);
    });

    connect(mBasicProperties, &BasicPropertiesWidget::isFollowUpdated, this, [this](bool follow) {
        if (!mDataPtr) { return; }
        follow ? mChildFlag.set(Ptcl::ChildFlag::IsFollow) : mChildFlag.clear(Ptcl::ChildFlag::IsFollow);
        emit flagsUpdated(mChildFlag);
    });

    connect(mBasicProperties, &BasicPropertiesWidget::isParentFieldUpdated, this, [this](bool field) {
        if (!mDataPtr) { return; }
        field ? mChildFlag.set(Ptcl::ChildFlag::ParentField) : mChildFlag.clear(Ptcl::ChildFlag::ParentField);
        emit flagsUpdated(mChildFlag);
    });

    connect(mBasicProperties, &BasicPropertiesWidget::isPolygonUpdated, this, [this](bool polygon) {
        if (!mDataPtr) { return; }
        polygon ? mChildFlag.set(Ptcl::ChildFlag::IsPolygon) : mChildFlag.clear(Ptcl::ChildFlag::IsPolygon);
        emit flagsUpdated(mChildFlag);
    });

    connect(mBasicProperties, &BasicPropertiesWidget::isPreDrawUpdated, this, [this](bool preDraw) {
        if (!mDataPtr) { return; }
        preDraw ? mChildFlag.set(Ptcl::ChildFlag::PreChildDraw) : mChildFlag.clear(Ptcl::ChildFlag::PreChildDraw);
        emit flagsUpdated(mChildFlag);
    });

    // Emission Properties
    connect(mEmissionProperties, &EmissionPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::EmissionProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setEmissionProperties(properties);
    });

    // Velocity Properties
    connect(mVelocityProperties, &VelocityPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::VelocityProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setVelocityProperties(properties);
    });

    connect(mVelocityProperties, &VelocityPropertiesWidget::inheritVelUpdated, this, [this](bool inherit) {
        if (!mDataPtr) { return; }
        inherit ? mChildFlag.set(Ptcl::ChildFlag::VelInherit) : mChildFlag.clear(Ptcl::ChildFlag::VelInherit);
        emit flagsUpdated(mChildFlag);
    });

    // Rotation Properties
    connect(mRotationProperties, &RotationPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::RotationProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setRotationProperties(properties);
    });

    connect(mRotationProperties, &RotationPropertiesWidget::inheritRotationUpdated, this, [this](bool inherit) {
        if (!mDataPtr) { return; }
        inherit ? mChildFlag.set(Ptcl::ChildFlag::RotateInherit) : mChildFlag.clear(Ptcl::ChildFlag::RotateInherit);
        emit flagsUpdated(mChildFlag);
    });

    // Scale Properties
    connect(mScaleProperties, &ScalePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::ScaleProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setScaleProperties(properties);
    });

    connect(mScaleProperties, &ScalePropertiesWidget::inheritScaleUpdated, this, [this](bool inherit) {
        if (!mDataPtr) { return; }
        inherit ? mChildFlag.set(Ptcl::ChildFlag::ScaleInherit) : mChildFlag.clear(Ptcl::ChildFlag::ScaleInherit);
        emit flagsUpdated(mChildFlag);
    });

    // Texture Properties
    connect(mTextureProperties, &TexturePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::TextureProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setTextureProperties(properties);
    });

    connect(mTextureProperties, &TexturePropertiesWidget::textureUpdated, this, [this](const std::shared_ptr<Ptcl::Texture>& oldTexture, const std::shared_ptr<Ptcl::Texture>& newTexture) {
        if (!mDataPtr) { return; }
        mDataPtr->setTexture(newTexture);

        emit textureUpdated(oldTexture, newTexture);
        mCombinerProperties->updateCombinerPreview();
    });

    // Color Properties
    connect(mColorProperties, &ColorPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::ColorProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setColorProperties(properties);
        mCombinerProperties->updateCombinerPreview();
    });

    connect(mColorProperties, &ColorPropertiesWidget::inheritColorUpdated, this, [this](bool inherit) {
        if (!mDataPtr) { return; }
        inherit ? mChildFlag.set(Ptcl::ChildFlag::Color0Inherit) : mChildFlag.clear(Ptcl::ChildFlag::Color0Inherit);
        emit flagsUpdated(mChildFlag);
        setCombinerPropertiesSrc();
    });

    // Alpha Properties
    connect(mAlphaProperties, &AlphaPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::AlphaProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setAlphaProperties(properties);
    });

    connect(mAlphaProperties, &AlphaPropertiesWidget::inheritAlphaUpdated, this, [this](bool inherit) {
        if (!mDataPtr) { return; }
        inherit ? mChildFlag.set(Ptcl::ChildFlag::AlphaInherit) : mChildFlag.clear(Ptcl::ChildFlag::AlphaInherit);
        emit flagsUpdated(mChildFlag);
    });

    // Combiner Properties
    connect(mCombinerProperties, &CombinerPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ChildData::CombinerProperties& properties) {
        if (!mDataPtr) { return; }
        mDataPtr->setCombinerProperties(properties);
    });
}

void ChildEditorWidget::setChildData(Ptcl::ChildData* childData, const BitFlag<Ptcl::ChildFlag>& childFlag) {
    QSignalBlocker b1(mBasicProperties);
    QSignalBlocker b2(mEmissionProperties);
    QSignalBlocker b3(mVelocityProperties);
    QSignalBlocker b4(mRotationProperties);
    QSignalBlocker b5(mScaleProperties);
    QSignalBlocker b6(mTextureProperties);
    QSignalBlocker b7(mColorProperties);
    QSignalBlocker b8(mAlphaProperties);
    QSignalBlocker b9(mCombinerProperties);

    mDataPtr = childData;
    mChildFlag = childFlag;

    mBasicProperties->setProperties(mDataPtr->basicProperties(), mChildFlag.isSet(Ptcl::ChildFlag::IsFollow), mChildFlag.isSet(Ptcl::ChildFlag::ParentField), mChildFlag.isSet(Ptcl::ChildFlag::PreChildDraw));
    mEmissionProperties->setProperties(mDataPtr->emissionProperties());
    mVelocityProperties->setProperties(mDataPtr->velocityProperties(), mChildFlag.isSet(Ptcl::ChildFlag::VelInherit));
    mRotationProperties->setProperties(mDataPtr->rotationProperties(), mChildFlag.isSet(Ptcl::ChildFlag::RotateInherit));
    mScaleProperties->setProperties(mDataPtr->scaleProperties(), mChildFlag.isSet(Ptcl::ChildFlag::ScaleInherit));
    mTextureProperties->setProperties(mDataPtr->textureProperties(), mDataPtr->textureHandle().get());
    mColorProperties->setProperties(mDataPtr->colorProperties(), mChildFlag.isSet(Ptcl::ChildFlag::Color0Inherit));
    mAlphaProperties->setProperties(mDataPtr->alphaProperties(), mChildFlag.isSet(Ptcl::ChildFlag::AlphaInherit));
    mCombinerProperties->setProperties(mDataPtr->combinerProperties());
    setCombinerPropertiesSrc();

    const bool isEnabled = mChildFlag.isSet(Ptcl::ChildFlag::Enabled);
    mEnabledCheckbox.setChecked(isEnabled);
    mSectionsContainer->setEnabled(isEnabled);

    setEnabled(true);
}

void ChildEditorWidget::setTextureList(const Ptcl::TextureList* textureList) {
    mTextureList = textureList;
    mTextureProperties->setTextureList(textureList);
}

void ChildEditorWidget::setParentColor0(const Ptcl::binColor4f& parentColor0) {
    mParentColor0 = parentColor0;
    mCombinerProperties->updateCombinerPreview();
};

void ChildEditorWidget::setCombinerPropertiesSrc() {
    if (mChildFlag.isSet(Ptcl::ChildFlag::Color0Inherit)) {
        mCombinerProperties->setCombinerSrc(&mDataPtr->textureHandle(), &mDataPtr->colorProperties().color1, &mParentColor0);
    } else {
        mCombinerProperties->setCombinerSrc(&mDataPtr->textureHandle(), &mDataPtr->colorProperties().color1, &mDataPtr->colorProperties().color0);
    }
    mCombinerProperties->updateCombinerPreview();
}

void ChildEditorWidget::clear() {
    setEnabled(false);
    mDataPtr = nullptr;
}


// ========================================================================== //


} // namespace PtclEditor
