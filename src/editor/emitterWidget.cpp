#include "editor/emitterWidget.h"

namespace PtclEditor {

EmitterWidget::EmitterWidget(QWidget* parent) :
    QWidget(parent) {

    setLayout(&mMainLayout);

    mVolumeTypeLabel.setText("VolumeType:");
    mVolumeRadiusLabel.setText("VolumeRadius:");

    QStringList emitterTypes{"Simple", "Complex", "UnkType2"};
    mTypeComboBox.addItems(emitterTypes);
    mMainLayout.addWidget(new QLabel("Emitter Type:"), 0, 0);
    mMainLayout.addWidget(&mTypeComboBox,              0, 1);

    mMainLayout.addWidget(new QLabel("Flag:"), 1, 0);
    mMainLayout.addWidget(&mFlagSpinBox,       1, 1);

    mMainLayout.addWidget(new QLabel("Random Seed:"), 2, 0);
    mMainLayout.addWidget(&mRandomSeedSpinBox,        2, 1);

    mMainLayout.addWidget(new QLabel("Name:"), 3, 0);
    mMainLayout.addWidget(&mNameLineEdit,      3, 1);

    mMainLayout.addWidget(&mImageLabel,     4, 0);
    mMainLayout.addWidget(&mImageInfoLabel, 4, 1);

    mMainLayout.addWidget(new QLabel("_2C:"), 5, 0);
    mMainLayout.addWidget(&m_2CSpinBox,       5, 1);

    mMainLayout.addWidget(new QLabel("_2D:"), 6, 0);
    mMainLayout.addWidget(&m_2DSpinBox,       6, 1);

    mMainLayout.addWidget(new QLabel("_2E:"), 7, 0);
    mMainLayout.addWidget(&m_2ESpinBox,       7, 1);

    mMainLayout.addWidget(new QLabel("_2F:"), 8, 0);
    mMainLayout.addWidget(&m_2FSpinBox,       8, 1);

    mMainLayout.addWidget(new QLabel("_30:"), 9, 0);
    mMainLayout.addWidget(&m_30SpinBox,       9, 1);

    mMainLayout.addWidget(new QLabel("_31:"), 10, 0);
    mMainLayout.addWidget(&m_31SpinBox,       10, 1);

    mMainLayout.addWidget(new QLabel("_32:"), 11, 0);
    mMainLayout.addWidget(&m_32SpinBox,       11, 1);

    mMainLayout.addWidget(&mVolumeTypeLabel,   12, 0);
    mMainLayout.addWidget(&mVolumeRadiusLabel, 13, 0);

    m_44SpinBox.setMaximum(std::numeric_limits<f32>::max());
    m_44SpinBox.setMinimum(std::numeric_limits<f32>::min());
    mMainLayout.addWidget(new QLabel("_44:"), 14, 0);
    mMainLayout.addWidget(&m_44SpinBox,       14, 1);

    m_48SpinBox.setMaximum(std::numeric_limits<f32>::max());
    m_48SpinBox.setMinimum(std::numeric_limits<f32>::min());
    mMainLayout.addWidget(new QLabel("_48:"), 15, 0);
    mMainLayout.addWidget(&m_48SpinBox,       15, 1);

    m_4CSpinBox.setMaximum(std::numeric_limits<f32>::max());
    m_4CSpinBox.setMinimum(std::numeric_limits<f32>::min());
    mMainLayout.addWidget(new QLabel("_4C:"), 16, 0);
    mMainLayout.addWidget(&m_4CSpinBox,       16, 1);

    mMainLayout.addWidget(new QLabel("PtclLife:"), 17, 0);
    mMainLayout.addWidget(&mPtclLifeSpinBox,       17, 1);

    mMainLayout.addWidget(new QLabel("_88:"), 18, 0);
    mMainLayout.addWidget(&m_88SpinBox,       18, 1);

    m_8CSpinBox.setMaximum(std::numeric_limits<f32>::max());
    m_8CSpinBox.setMinimum(std::numeric_limits<f32>::min());
    mMainLayout.addWidget(new QLabel("_8C:"), 19, 0);
    mMainLayout.addWidget(&m_8CSpinBox,       19, 1);

}

void EmitterWidget::setEmitter(Ptcl::Emitter* emitter) {

    mEmitterPtr = emitter;
    // TODO: Update stuff...

    mTypeComboBox.setCurrentIndex(static_cast<s32>(mEmitterPtr->type()));
    mFlagSpinBox.setValue(mEmitterPtr->flag());
    mRandomSeedSpinBox.setValue(mEmitterPtr->randomSeed());
    mNameLineEdit.setText(mEmitterPtr->name());

    // TODO: Move this...
    if (mEmitterPtr->textureHandle().isValid()) {
        QImage texture = mEmitterPtr->textureHandle().get()->textureData();
        auto format = mEmitterPtr->textureHandle().get()->textureFormat();

        qDebug() << "Texture User Count: " << mEmitterPtr->textureHandle()->userCount();

        auto width = texture.width();
        auto height = texture.height();

        QPixmap pixmap = QPixmap::fromImage(texture);
        mImageLabel.setPixmap(pixmap.scaled(width * 3, height * 3, Qt::KeepAspectRatio));

        QString labelText = QString("Format: %1 \nWidth: %2 \nHeight: %3").arg(toQString(format)).arg(width).arg(height);
        mImageInfoLabel.setText(labelText);
    }

    // More Stuff
    m_2CSpinBox.setValue(mEmitterPtr->_2C());
    m_2DSpinBox.setValue(mEmitterPtr->_2D());
    m_2ESpinBox.setValue(mEmitterPtr->_2E());
    m_2FSpinBox.setValue(mEmitterPtr->_2F());
    m_30SpinBox.setValue(mEmitterPtr->_30());
    m_31SpinBox.setValue(mEmitterPtr->_31());
    m_32SpinBox.setValue(mEmitterPtr->_32());
    mVolumeTypeLabel.setText("VolumeType: " + Ptcl::toString(mEmitterPtr->volumeType()));
    mVolumeRadiusLabel.setText(QString("VolumeRadius: {%1, %2, %3}").arg(mEmitterPtr->volumeRadius().x()).arg(mEmitterPtr->volumeRadius().y()).arg(mEmitterPtr->volumeRadius().z()));

    m_44SpinBox.setValue(mEmitterPtr->_44());
    m_48SpinBox.setValue(mEmitterPtr->_48());
    m_4CSpinBox.setValue(mEmitterPtr->_4C());

    mPtclLifeSpinBox.setValue(mEmitterPtr->ptclLife());
    m_88SpinBox.setValue(mEmitterPtr->_88());
    m_8CSpinBox.setValue(mEmitterPtr->_8C());
}

} // namespace PtclEditor
