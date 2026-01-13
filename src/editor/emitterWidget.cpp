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

    addLabledWidget(&mTypeComboBox, "Emitter Type:", 0, 0, 3);

    // Random Seed
    mRandomSeedMode.addItem("Random Per Emitter", static_cast<int>(PtclSeed::Mode::RandomPerEmitter));
    mRandomSeedMode.addItem("Random Per EmitterSet", static_cast<int>(PtclSeed::Mode::RandomPerSet));
    mRandomSeedMode.addItem("Constant Seed", static_cast<int>(PtclSeed::Mode::ConstantSeed));
    connect(&mRandomSeedMode, &QComboBox::currentIndexChanged, this, [=, this]() {
        if (!mEmitterPtr || mIsPopulating) {
            return;
        }

        auto& seed = mEmitterPtr->randomSeed();
        auto mode = static_cast<PtclSeed::Mode>(mRandomSeedMode.currentData().toUInt());

        seed.setMode(mode);
        mRandomSeedSpinBox.setVisible(mode == PtclSeed::Mode::ConstantSeed);
    });

    mRandomSeedSpinBox.setVisible(false);
    mRandomSeedSpinBox.setRange(1, 0xFFFFFFFE);
    connect(&mRandomSeedSpinBox, &SizedSpinBoxBase::valueChanged, this, [=, this](int value) {
        if (!mEmitterPtr || mIsPopulating) {
            return;
        }

        auto& seed = mEmitterPtr->randomSeed();
        if (seed.mode() == PtclSeed::Mode::ConstantSeed) {
            seed.setConstantSeed(static_cast<u32>(value));
        }
    });

    addLabledWidget(&mRandomSeedMode, "RNG Mode:", 2, 0, 3);
    mMainLayout.addWidget(&mRandomSeedSpinBox, 2, 3, 1, 1);

    // Name
    mNameLineEdit.setValidator(new EmitterNameValidator(&mNameLineEdit));
    connect(&mNameLineEdit, &QLineEdit::textChanged, this, [=, this](const QString& text) {
        if (!mEmitterPtr || mIsPopulating) {
            return;
        }

        mEmitterPtr->setName(text);
        emit nameUpdated(text);
    });
    addLabledWidget(&mNameLineEdit, "Name:", 4, 0, 3);

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
    addLabledWidget(&m_2FSpinBox, "_2F:",  9, 0, 3);
    addLabledWidget(&m_30SpinBox, "_30:",  10, 0, 3);
    addLabledWidget(&m_31SpinBox, "_31:", 11, 0, 3);
    addLabledWidget(&m_31SpinBox, "_33:", 13, 0, 3);

    // Volume Properties
    auto volumePropertiesSection = new CollapsibleWidget("Volume properties", this);
    volumePropertiesSection->setContent(&mVolumeProperties);
    mMainLayout.addWidget(volumePropertiesSection, 14, 0, 1, 4);

    mFigureVelSpinBox.setMaximum(std::numeric_limits<f32>::max());
    mFigureVelSpinBox.setMinimum(std::numeric_limits<f32>::min());
    addLabledWidget(&mFigureVelSpinBox, "Figure Velocity:", 18, 0, 3);

    addLabledWidget(&mEmitterVelDirSpinBox, "Emitter Velocity Direction:", 19, 0, 3);
    addLabledWidget(&m_5CSpinBox,           "_5C:",                        20, 0, 3);
    addLabledWidget(&mInitVelRndSpinBox,    "Initial Velocity Random:",    21, 0, 3);
    addLabledWidget(&mSpreadVecSpinBox,     "Spread Vector:",              22, 0, 3);

    // _70

    addLabledWidget(&m_80SpinBox,          "_80:",              23, 0, 3);
    addLabledWidget(&mPtclLifeSpinBox,     "Ptcl Life:",        24, 0, 3);
    addLabledWidget(&mPtclLifeRandSpinBox, "Ptcl Life Random:", 25, 0, 3);

    m_8CSpinBox.setMaximum(std::numeric_limits<f32>::max());
    m_8CSpinBox.setMinimum(std::numeric_limits<f32>::min());
    addLabledWidget(&m_8CSpinBox, "_8C:", 26, 0, 3);

    addLabledWidget(&m_90SpinBox,        "_90:",            27, 0, 3);
    addLabledWidget(&mBillboardComboBox, "Billboard Type:", 28, 0, 3);
    addLabledWidget(&m_98SpinBox,        "_98:",            29, 0, 3);
    addLabledWidget(&m_9CSpinBox,        "_9C:",            30, 0, 3);
    addLabledWidget(&m_A0SpinBox,        "_A0:",            31, 0, 3);
    addLabledWidget(&m_A4SpinBox,        "_A4:",            32, 0, 3);

    // Color Properties
    auto colorPropertiesSection = new CollapsibleWidget("Color properties", this);
    colorPropertiesSection->setContent(&mColorProperties);
    mMainLayout.addWidget(colorPropertiesSection, 33, 0, 1, 4);

    // _D8

    addLabledWidget(&mInitAlphaSpinBox, "Initial Alpha:", 38, 0, 3);
    addLabledWidget(&mDiffAlpha21SpinBox, "Diff Alpha21:", 39, 0, 3);
    addLabledWidget(&mDiffAlpha32SpinBox, "Diff Alpha32:", 40, 0, 3);
    addLabledWidget(&mAlphaSection1SpinBox, "Alpha Section 1:", 41, 0, 3);
    addLabledWidget(&mAlphaSection2SpinBox, "Alpha Section 2:", 42, 0, 3);

    // Rotation Properties
    connect(&mRotationProperties, &RotationPropertiesWidget::propertiesUpdated, this, [this](const Ptcl::RotationProperties& properties) {
        if (!mEmitterPtr) { return; }
        mEmitterPtr->setRotationProperties(properties);
    });

    auto rotationPropertiesSection = new CollapsibleWidget("Rotation properties", this);
    rotationPropertiesSection->setContent(&mRotationProperties);
    mMainLayout.addWidget(rotationPropertiesSection, 49, 0, 1, 4);


    addLabledWidget(&mCombinerTypeSpinBox, "Combiner Type:", 50, 0, 3);
    addLabledWidget(&mFollowTypeSpinBox, "Follow Type:", 51, 0, 3);
    addLabledWidget(&m_134SpinBox, "_134:", 52, 0, 3);

    // Scale Properties
    connect(&mScaleProperties, &scalePropertiesWidget::propertiesUpdated, this, [this](const Ptcl::ScaleProperties& properties) {
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
    // TODO: Update stuff...

    mTypeComboBox.setCurrentEnum(mEmitterPtr->type());

    // Random Seed
    auto& randomSeed = mEmitterPtr->randomSeed();
    auto seedMode = randomSeed.mode();
    int index = mRandomSeedMode.findData(static_cast<int>(seedMode));
    if (index != -1) {
        mRandomSeedMode.setCurrentIndex(index);
    }
    mRandomSeedSpinBox.setValue(randomSeed.constantSeed());
    mRandomSeedSpinBox.setVisible(seedMode == PtclSeed::Mode::ConstantSeed);

    mNameLineEdit.setText(mEmitterPtr->name());

    mTextureProperties.setEmitter(mEmitterPtr);

    // More Stuff
    m_2CSpinBox.setValue(mEmitterPtr->_2C());
    m_2DSpinBox.setValue(mEmitterPtr->_2D());
    m_2ESpinBox.setValue(mEmitterPtr->_2E());
    m_2FSpinBox.setValue(mEmitterPtr->_2F());
    m_30SpinBox.setValue(mEmitterPtr->_30());
    m_31SpinBox.setValue(mEmitterPtr->_31());

    m_33SpinBox.setValue(mEmitterPtr->_33());

    mVolumeProperties.setEmitter(mEmitterPtr);

    mFigureVelSpinBox.setValue(mEmitterPtr->figureVel());
    mEmitterVelDirSpinBox.setVector(mEmitterPtr->emitterVelDir());
    m_5CSpinBox.setValue(mEmitterPtr->_5C());
    mInitVelRndSpinBox.setValue(mEmitterPtr->initVelRnd());
    mSpreadVecSpinBox.setVector(mEmitterPtr->spreadVec());
    m_80SpinBox.setValue(mEmitterPtr->_80());
    mPtclLifeSpinBox.setValue(mEmitterPtr->ptclLife());
    mPtclLifeRandSpinBox.setValue(mEmitterPtr->ptclLifeRnd());
    m_8CSpinBox.setValue(mEmitterPtr->_8C());
    m_90SpinBox.setValue(mEmitterPtr->_90());
    mBillboardComboBox.setCurrentEnum((mEmitterPtr->billboardType()));
    m_9CSpinBox.setValue(mEmitterPtr->_9C());
    m_A0SpinBox.setValue(mEmitterPtr->_A0());
    m_A4SpinBox.setValue(mEmitterPtr->_A4());

    mColorProperties.setEmitter(mEmitterPtr);

    mInitAlphaSpinBox.setValue(mEmitterPtr->initAlpha());
    mDiffAlpha21SpinBox.setValue(mEmitterPtr->diffAlpha21());
    mDiffAlpha32SpinBox.setValue(mEmitterPtr->diffAlpha32());
    mAlphaSection1SpinBox.setValue(mEmitterPtr->alphaSection1());
    mAlphaSection2SpinBox.setValue(mEmitterPtr->alphaSection2());

    mScaleProperties.setProperties(mEmitterPtr->scaleProperties());

    mCombinerTypeSpinBox.setCurrentEnum(mEmitterPtr->combinerType());
    mFollowTypeSpinBox.setCurrentEnum(mEmitterPtr->followType());
    m_134SpinBox.setValue(mEmitterPtr->_134());

    mRotationProperties.setProperties(mEmitterPtr->rotationProperties());

    mIsPopulating = false;
}

TexturePropertiesWidget& EmitterWidget::getTextureProperties() {
    return mTextureProperties;
}


// ========================================================================== //


} // namespace PtclEditor
