#include "editor/emitterWidget.h"

#include <QScrollArea>

namespace PtclEditor {

EmitterWidget::EmitterWidget(QWidget* parent) :
    QWidget(parent) {

    QWidget* containerWidget = new QWidget;
    containerWidget->setLayout(&mMainLayout);

    containerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
    containerWidget->setMinimumWidth(400);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidget(containerWidget);
    scrollArea->setWidgetResizable(true);

    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Outer layout to hold the scroll area
    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->addWidget(scrollArea);
    outerLayout->setContentsMargins(0, 0, 0, 0);  // optional
    setLayout(outerLayout);

    // Optional: make this widget expand within its parent
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    mMainLayout.setColumnStretch(0, 0); // labels
    mMainLayout.setColumnStretch(1, 1); // widgets
    mMainLayout.setColumnStretch(2, 0); // optional 2nd label
    mMainLayout.setColumnStretch(3, 1); // optional 2nd widget

    auto addLabledWidget = [&](QWidget* widget, const QString& label, int row, int column, int span = 1) {
        mMainLayout.addWidget(new QLabel(label), row, column);
        mMainLayout.addWidget(widget,            row, column + 1, 1, span);
    };

    addLabledWidget(&mTypeComboBox, "Emitter Type:", 0, 0, 3);
    addLabledWidget(&mFlagSpinBox, "Flag:", 1, 0, 3);
    addLabledWidget(&mRandomSeedSpinBox, "Random Seed:", 2, 0, 3);
    addLabledWidget(&mNameLineEdit, "Name:", 3, 0, 3);

    mMainLayout.addWidget(&mTextureProperties, 4, 0, 1, 3);
    connect(&mTextureProperties, &TexturePropertiesWidget::textureUpdated, this, [=, this](int oldIndex, int index) {
        emit textureUpdated(oldIndex, index);
    });

    addLabledWidget(&m_2CSpinBox, "_2C:",  5, 0, 3);
    addLabledWidget(&m_2DSpinBox, "_2D:",  6, 0, 3);
    addLabledWidget(&m_2ESpinBox, "_2E:",  7, 0, 3);
    addLabledWidget(&m_2FSpinBox, "_2F:",  8, 0, 3);
    addLabledWidget(&m_30SpinBox, "_30:",  9, 0, 3);
    addLabledWidget(&m_31SpinBox, "_31:", 10, 0, 3);
    addLabledWidget(&m_32SpinBox, "_32:", 11, 0, 3);

    addLabledWidget(&mVolumeTypeComboBox,  "Volume Type:",   12, 0, 3);
    addLabledWidget(&mVolumeRadiusSpinBox, "Volume Radius:", 13, 0, 3);

    m_44SpinBox.setMaximum(std::numeric_limits<f32>::max());
    m_44SpinBox.setMinimum(std::numeric_limits<f32>::min());
    addLabledWidget(&m_44SpinBox, "_44:", 14, 0, 3);

    m_48SpinBox.setMaximum(std::numeric_limits<f32>::max());
    m_48SpinBox.setMinimum(std::numeric_limits<f32>::min());
    addLabledWidget(&m_48SpinBox, "_48:", 15, 0, 3);

    mFigureVelSpinBox.setMaximum(std::numeric_limits<f32>::max());
    mFigureVelSpinBox.setMinimum(std::numeric_limits<f32>::min());
    addLabledWidget(&mFigureVelSpinBox, "Figure Velocity:", 16, 0, 3);

    addLabledWidget(&mEmitterVelDirSpinBox, "Emitter Velocity Direction:", 17, 0, 3);
    addLabledWidget(&m_5CSpinBox,           "_5C:",                        18, 0, 3);
    addLabledWidget(&mInitVelRndSpinBox,    "Initial Velocity Random:",    19, 0, 3);
    addLabledWidget(&mSpreadVecSpinBox,     "Spread Vector:",              20, 0, 3);

    // _70

    addLabledWidget(&m_80SpinBox,          "_80:",              21, 0, 3);
    addLabledWidget(&mPtclLifeSpinBox,     "Ptcl Life:",        22, 0, 3);
    addLabledWidget(&mPtclLifeRandSpinBox, "Ptcl Life Random:", 23, 0, 3);

    m_8CSpinBox.setMaximum(std::numeric_limits<f32>::max());
    m_8CSpinBox.setMinimum(std::numeric_limits<f32>::min());
    addLabledWidget(&m_8CSpinBox, "_8C:", 24, 0, 3);

    addLabledWidget(&m_90SpinBox,        "_90:",            25, 0, 3);
    addLabledWidget(&mBillboardComboBox, "Billboard Type:", 26, 0, 3);
    addLabledWidget(&m_98SpinBox,        "_98:",            27, 0, 3);
    addLabledWidget(&m_9CSpinBox,        "_9C:",            28, 0, 3);
    addLabledWidget(&m_A0SpinBox,        "_A0:",            29, 0, 3);
    addLabledWidget(&m_A4SpinBox,        "_A4:",            30, 0, 3);

    for (int i = 0; i < mColorWidgets.size(); ++i) {
        addLabledWidget(&mColorWidgets[i], QString("Color %1:").arg(i), 31 + i, 0, 3);
        mColorWidgets[i].setProperty("colorIndex", i);
        connect(&mColorWidgets[i], &RGBAColorWidget::colorChanged, this, &EmitterWidget::handleColorChanged);
    }

    // _D8

    addLabledWidget(&mColorSection1SpinBox,  "Color Section 1:",   34, 0, 3);
    addLabledWidget(&mColorSection2SpinBox,  "Color Section 2:",   35, 0, 3);
    addLabledWidget(&mColorSection3SpinBox,  "Color Section 3:",   36, 0, 3);
    addLabledWidget(&mColorNumRepeatSpinBox, "Color Num Reapeat:", 37, 0, 3);

    addLabledWidget(&mInitAlphaSpinBox, "Initial Alpha:", 38, 0, 3);
    addLabledWidget(&mDiffAlpha21SpinBox, "Diff Alpha21:", 39, 0, 3);
    addLabledWidget(&mDiffAlpha32SpinBox, "Diff Alpha32:", 40, 0, 3);
    addLabledWidget(&mAlphaSection1SpinBox, "Alpha Section 1:", 41, 0, 3);
    addLabledWidget(&mAlphaSection2SpinBox, "Alpha Section 2:", 42, 0, 3);

    addLabledWidget(&mInitScaleSpinBox, "Initial Scale:", 43, 0, 3);
    addLabledWidget(&mDiffScale21SpinBox, "Diff Scale 21:", 44, 0, 3);
    addLabledWidget(&mDiffScale32SpinBox, "Diff Scale 32:", 45, 0, 3);
    addLabledWidget(&mScaleSection1SpinBox, "Scale Section 1:", 46, 0, 3);
    addLabledWidget(&mScaleSection2SpinBox, "Scale Section 2:", 47, 0, 3);
    addLabledWidget(&mScaleRandSpinBox, "Scale Random:", 48, 0, 3);

    addLabledWidget(&m_12CSpinBox, "_12C:", 49, 0, 3);
    addLabledWidget(&m_130SpinBox, "_130:", 50, 0, 3);
    addLabledWidget(&m_134SpinBox, "_134:", 51, 0, 3);

    addLabledWidget(&mInitRotSpinBox, "Initial Rotation:", 52, 0, 3);
    addLabledWidget(&mInitRotRandSpinBox, "Initial Rotation Random:", 53, 0, 3);
    addLabledWidget(&mRotVelSpinBox, "Rotation Velocity:", 54, 0, 3);
    addLabledWidget(&mRotVelRandSpinBox, "Rotation Velocity Random:", 55, 0, 3);
}

void EmitterWidget::setEmitter(Ptcl::Emitter* emitter) {

    mEmitterPtr = emitter;
    // TODO: Update stuff...

    mTypeComboBox.setCurrentEnum(mEmitterPtr->type());
    mFlagSpinBox.setValue(mEmitterPtr->flag());
    mRandomSeedSpinBox.setValue(mEmitterPtr->randomSeed());
    mNameLineEdit.setText(mEmitterPtr->name());

    mTextureProperties.setEmitter(mEmitterPtr);

    // More Stuff
    m_2CSpinBox.setValue(mEmitterPtr->_2C());
    m_2DSpinBox.setValue(mEmitterPtr->_2D());
    m_2ESpinBox.setValue(mEmitterPtr->_2E());
    m_2FSpinBox.setValue(mEmitterPtr->_2F());
    m_30SpinBox.setValue(mEmitterPtr->_30());
    m_31SpinBox.setValue(mEmitterPtr->_31());

    mVolumeTypeComboBox.setCurrentEnum(mEmitterPtr->volumeType());
    mVolumeRadiusSpinBox.setVector(mEmitterPtr->volumeRadius());

    m_44SpinBox.setValue(mEmitterPtr->_44());
    m_48SpinBox.setValue(mEmitterPtr->_48());
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

    for (int i = 0; i < mColorWidgets.size(); ++i) {
        mColorWidgets[i].setColor(mEmitterPtr->colors()[i]);
    }

    mColorSection1SpinBox.setValue(mEmitterPtr->colorSection1());
    mColorSection2SpinBox.setValue(mEmitterPtr->colorSection2());
    mColorSection3SpinBox.setValue(mEmitterPtr->colorSection3());
    mColorNumRepeatSpinBox.setValue(mEmitterPtr->colorNumRepeat());

    mInitAlphaSpinBox.setValue(mEmitterPtr->initAlpha());
    mDiffAlpha21SpinBox.setValue(mEmitterPtr->diffAlpha21());
    mDiffAlpha32SpinBox.setValue(mEmitterPtr->diffAlpha32());
    mAlphaSection1SpinBox.setValue(mEmitterPtr->alphaSection1());
    mAlphaSection2SpinBox.setValue(mEmitterPtr->alphaSection2());

    mInitScaleSpinBox.setVector(mEmitterPtr->initScale());
    mDiffScale21SpinBox.setVector(mEmitterPtr->diffScale21());
    mDiffScale32SpinBox.setVector(mEmitterPtr->diffScale32());
    mScaleSection1SpinBox.setValue(mEmitterPtr->scaleSection1());
    mScaleSection2SpinBox.setValue(mEmitterPtr->scaleSection2());
    mScaleRandSpinBox.setValue(mEmitterPtr->scaleRand());

    m_12CSpinBox.setValue(mEmitterPtr->_12C());
    m_130SpinBox.setValue(mEmitterPtr->_130());
    m_134SpinBox.setValue(mEmitterPtr->_134());

    mInitRotSpinBox.setVector(mEmitterPtr->initRot());
    mInitRotRandSpinBox.setVector(mEmitterPtr->initRotRand());
    mRotVelSpinBox.setVector(mEmitterPtr->rotVel());
    mRotVelRandSpinBox.setVector(mEmitterPtr->rotVelRand());
}

TexturePropertiesWidget& EmitterWidget::getTextureProperties() {
    return mTextureProperties;
}

void EmitterWidget::handleColorChanged() {
    auto* colorWidget = qobject_cast<RGBAColorWidget*>(sender());

    if (!colorWidget) {
        return;
    }

    bool ok = false;
    int index = colorWidget->property("colorIndex").toInt(&ok);

    if (!ok) {
        qWarning() << "colorIndex propery missing or invalid";
        return;
    }

    mEmitterPtr->setColor(index, colorWidget->color());
}

} // namespace PtclEditor
