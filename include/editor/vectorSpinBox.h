#pragma once

#include "typedefs.h"

#include <QBoxLayout>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QWidget>


// ========================================================================== //


class VectorSpinBoxBase : public QWidget {
    Q_OBJECT
public:
    explicit VectorSpinBoxBase(QWidget* parent = nullptr) :
        QWidget{parent} {}

    void setOrientation(Qt::Orientation orientation) {
        if (!mMainLayout) {
            return;
        }

        mMainLayout->setDirection(orientation == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);
    }

signals:
    void valueChanged();

protected:
    inline void setupUI(u32 axisCount) {
        mMainLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
        mMainLayout->setContentsMargins(mMainLayout->contentsMargins().left(), 0, 0, 0);
        setLayout(mMainLayout);

        std::tie(mLabelX, mSpinBoxX) = createAxis("X");
        std::tie(mLabelY, mSpinBoxY) = createAxis("Y");

        connect(mSpinBoxX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VectorSpinBoxBase::valueChanged);
        connect(mSpinBoxY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VectorSpinBoxBase::valueChanged);

        if (axisCount >= 3) {
            std::tie(mLabelZ, mSpinBoxZ) = createAxis("Z");
            connect(mSpinBoxZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VectorSpinBoxBase::valueChanged);
        }

        if (axisCount >= 4) {
            std::tie(mLabelW, mSpinBoxW) = createAxis("W");
            connect(mSpinBoxW, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VectorSpinBoxBase::valueChanged);
        }
    }

private:
    std::pair<QLabel*, QDoubleSpinBox*> createAxis(const QString& axisName) {
        // Spinbox
        auto* spinBox = new QDoubleSpinBox(this);
        spinBox->setRange(std::numeric_limits<f64>::lowest(), std::numeric_limits<f64>::max());
        spinBox->setDecimals(4);

        // Label
        auto* label = new QLabel(axisName + ":", this);
        label->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

        // Layout
        auto* axisLayout = new QHBoxLayout();
        axisLayout->setContentsMargins(0, 0, 0, 0);

        axisLayout->addWidget(label);
        axisLayout->addWidget(spinBox, 1);

        mMainLayout->addLayout(axisLayout);
        return {label, spinBox};
    }

protected:
    QLabel* mLabelX = nullptr;
    QLabel* mLabelY = nullptr;
    QLabel* mLabelZ = nullptr;
    QLabel* mLabelW = nullptr;

    QDoubleSpinBox* mSpinBoxX = nullptr;
    QDoubleSpinBox* mSpinBoxY = nullptr;
    QDoubleSpinBox* mSpinBoxZ = nullptr;
    QDoubleSpinBox* mSpinBoxW = nullptr;

    QBoxLayout* mMainLayout = nullptr;
};


// ========================================================================== //


template<typename T>
class VectorSpinBox final : public VectorSpinBoxBase {
    static_assert(std::is_same_v<T, QVector2D> ||
                  std::is_same_v<T, QVector3D> ||
                  std::is_same_v<T, QVector4D>,
                  "VectorSpinBox only supports QVector2D, QVector3D and QVector4D");
public:
    explicit VectorSpinBox(QWidget* parent = nullptr) :
        VectorSpinBoxBase(parent) {
        setupUI(mAxisCount);
    }

    void setVector(const T& vector) {
        mSpinBoxX->setValue(vector.x());
        mSpinBoxY->setValue(vector.y());

        if constexpr (mAxisCount >= 3) {
            mSpinBoxZ->setValue(vector.z());
        }

        if constexpr (mAxisCount >= 4) {
            mSpinBoxW->setValue(vector.w());
        }
    }

    T getVector() const {
        if constexpr (mAxisCount == 2) {
            return QVector2D(mSpinBoxX->value(), mSpinBoxY->value());
        } else if constexpr (mAxisCount == 3) {
            return QVector3D(mSpinBoxX->value(), mSpinBoxY->value(), mSpinBoxZ->value());
        } else {
            return QVector4D(mSpinBoxX->value(), mSpinBoxY->value(), mSpinBoxZ->value(), mSpinBoxW->value());
        }
    }

private:
    static constexpr u32 mAxisCount = std::is_same_v<T, QVector2D> ? 2 : std::is_same_v<T, QVector3D> ? 3 : 4;
};


// ========================================================================== //

