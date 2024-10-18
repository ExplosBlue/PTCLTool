#include "emitterwidget.h"

EmitterWidget::EmitterWidget(QWidget* parent) :
    QWidget(parent) {

    setLayout(&mMainLayout);

    // TEMP
    mTypeLabel.setText("EmitterType:");
    mFlagLabel.setText("Flag:");
    mFlagLabel.setText("RandomSeed:");
    mFlagLabel.setText("Name:");

    m_2CLabel.setText("_2C:");
    m_2DLabel.setText("_2D:");
    m_2ELabel.setText("_2E:");
    m_2FLabel.setText("_2F:");
    m_30Label.setText("_30:");
    m_31Label.setText("_31:");
    m_32Label.setText("_32:");

    mVolumeTypeLabel.setText("VolumeType:");
    mVolumeRadiusLabel.setText("VolumeRadius:");


    mMainLayout.addWidget(&mTypeLabel,       0, 0);
    mMainLayout.addWidget(&mFlagLabel,       1, 0);
    mMainLayout.addWidget(&mRandomSeedLabel, 2, 0);
    mMainLayout.addWidget(&mNameLabel,       3, 0);

    mMainLayout.addWidget(&mImageLabel,     4, 0);
    mMainLayout.addWidget(&mImageInfoLabel, 5, 0);

    mMainLayout.addWidget(&m_2CLabel,  6, 0);
    mMainLayout.addWidget(&m_2DLabel,  7, 0);
    mMainLayout.addWidget(&m_2ELabel,  8, 0);
    mMainLayout.addWidget(&m_2FLabel,  9, 0);
    mMainLayout.addWidget(&m_30Label, 10, 0);
    mMainLayout.addWidget(&m_31Label, 11, 0);
    mMainLayout.addWidget(&m_32Label, 12, 0);

    mMainLayout.addWidget(&mVolumeTypeLabel,   13, 0);
    mMainLayout.addWidget(&mVolumeRadiusLabel, 14, 0);

}

void EmitterWidget::setEmitter(Ptcl::Emitter* emitter) {
    mEmitterPtr = emitter;
    // TODO: Update stuff...

    // TODO: not this...
    mTypeLabel.setText("EmitterType: " + Ptcl::toString(mEmitterPtr->type()));
    mFlagLabel.setText("Flag: " + QString::number(mEmitterPtr->flag()));
    mRandomSeedLabel.setText("RandomSeed: " + QString::number(mEmitterPtr->randomSeed()));
    mNameLabel.setText("Name: " + mEmitterPtr->name());

    // TODO: Move this...
    QImage texture = mEmitterPtr->texture();

    auto format = mEmitterPtr->textureFormat();
    auto width = texture.width();
    auto height = texture.height();

    QPixmap pixmap = QPixmap::fromImage(texture);
    mImageLabel.setPixmap(pixmap.scaled(width * 3, height * 3, Qt::KeepAspectRatio));

    QString labelText = QString("Format: %1 \nWidth: %2 \nHeight: %3").arg(gr::toQString(format)).arg(width).arg(height);
    mImageInfoLabel.setText(labelText);

    // More Stuff
    m_2CLabel.setText("_2C: " + QString::number(mEmitterPtr->_2C()));
    m_2DLabel.setText("_2D: " + QString::number(mEmitterPtr->_2D()));
    m_2ELabel.setText("_2E: " + QString::number(mEmitterPtr->_2E()));
    m_2FLabel.setText("_2F: " + QString::number(mEmitterPtr->_2F()));
    m_30Label.setText("_30: " + QString::number(mEmitterPtr->_30()));
    m_31Label.setText("_31: " + QString::number(mEmitterPtr->_31()));
    m_32Label.setText("_32: " + QString::number(mEmitterPtr->_32()));
    mVolumeTypeLabel.setText("VolumeType: " + Ptcl::toString(mEmitterPtr->volumeType()));
    mVolumeRadiusLabel.setText(QString("VolumeRadius: {%1, %2, %3}").arg(mEmitterPtr->volumeRadius().x()).arg(mEmitterPtr->volumeRadius().y()).arg(mEmitterPtr->volumeRadius().z()));

}
