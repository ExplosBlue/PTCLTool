#include "editor/lifespanPropertiesWidget.h"

#include <QFormLayout>

// ========================================================================== //


LifespanPropertiesWidget::LifespanPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    mLifeSpanSpinBox.setRange(0, sLifeInfinite - 1);
    mLifeSpanSpinBox.setSuffix(" Frames");

    // TODO: unsure that this is actually treated as a percentage
    // Assumed based on behavior seen in EFT PTCL
    mLifeSpanRndSpinBox.setRange(0, std::numeric_limits<s32>::max());
    mLifeSpanRndSpinBox.setSuffix("%");

    mainLayout->addRow("Infinite Life", &mInfiniteLifeCheckBox);

    mainLayout->addRow("Lifespan:", &mLifeSpanSpinBox);
    mainLayout->addRow("Lifespan Randomness:", &mLifeSpanRndSpinBox);

    connect(&mLifeSpanSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        mProps.ptclLife = static_cast<s32>(value);
        emit propertiesUpdated(mProps);
    });

    connect(&mInfiniteLifeCheckBox, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
        QSignalBlocker b1(mLifeSpanSpinBox);

        const bool isInfinite = (state == Qt::CheckState::Checked);
        mProps.ptclLife = isInfinite ? sLifeInfinite : 100;
        mLifeSpanSpinBox.setValue(mProps.ptclLife);
        mLifeSpanSpinBox.setEnabled(!isInfinite);
        emit propertiesUpdated(mProps);
    });

    connect(&mLifeSpanRndSpinBox, &QSpinBox::valueChanged, this, [this](u64 value) {
        mProps.ptclLifeRnd = static_cast<s32>(value);
        emit propertiesUpdated(mProps);
    });
}

void LifespanPropertiesWidget::setProperties(const Ptcl::LifespanProperties& properties) {
    QSignalBlocker b1(mInfiniteLifeCheckBox);
    QSignalBlocker b2(mLifeSpanSpinBox);
    QSignalBlocker b3(mLifeSpanRndSpinBox);

    mProps = properties;

    const s32 lifeSpan = mProps.ptclLife;
    const bool infiniteLife = (lifeSpan == sLifeInfinite);

    mLifeSpanSpinBox.setValue(lifeSpan);
    mLifeSpanSpinBox.setDisabled(infiniteLife);
    mInfiniteLifeCheckBox.setChecked(infiniteLife);

    mLifeSpanRndSpinBox.setValue(mProps.ptclLifeRnd);
}


// ========================================================================== //
