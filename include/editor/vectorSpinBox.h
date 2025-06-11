#pragma once

#include "typedefs.h"

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QWidget>


// ========================================================================== //


class VectorSpinBoxBase : public QWidget
{
    Q_OBJECT
public:
    explicit VectorSpinBoxBase(QWidget* parent = nullptr) :
        QWidget{parent} {}

signals:
    void valueChanged();

protected:
    QLabel* mLabelX;
    QLabel* mLabelY;
    QLabel* mLabelZ;
    QLabel* mLabelW;

    QDoubleSpinBox* mSpinBoxX;
    QDoubleSpinBox* mSpinBoxY;
    QDoubleSpinBox* mSpinBoxZ;
    QDoubleSpinBox* mSpinBoxW;

    inline void setupUI(u32 axisCount) {
        auto* layout = new QHBoxLayout(this);

        mLabelX = new QLabel("X:", this);
        mSpinBoxX = new QDoubleSpinBox(this);
        mSpinBoxX->setRange(std::numeric_limits<f64>::min(), std::numeric_limits<f64>::max());
        mSpinBoxX->setDecimals(4);
        layout->addWidget(mLabelX);
        layout->addWidget(mSpinBoxX);

        mLabelY = new QLabel("Y:", this);
        mSpinBoxY = new QDoubleSpinBox(this);
        mSpinBoxY->setRange(std::numeric_limits<f64>::min(), std::numeric_limits<f64>::max());
        mSpinBoxY->setDecimals(4);
        layout->addWidget(mLabelY);
        layout->addWidget(mSpinBoxY);

        mLabelZ = new QLabel("Z:", this);
        mSpinBoxZ = new QDoubleSpinBox(this);
        mSpinBoxZ->setRange(std::numeric_limits<f64>::min(), std::numeric_limits<f64>::max());
        mSpinBoxZ->setDecimals(4);
        layout->addWidget(mLabelZ);
        layout->addWidget(mSpinBoxZ);
        if (axisCount < 3) {
            mLabelZ->setVisible(false);
            mSpinBoxZ->setVisible(false);
        }

        mLabelW = new QLabel("W:", this);
        mSpinBoxW = new QDoubleSpinBox(this);
        mSpinBoxW->setRange(std::numeric_limits<f64>::min(), std::numeric_limits<f64>::max());
        mSpinBoxW->setDecimals(4);
        layout->addWidget(mLabelW);
        layout->addWidget(mSpinBoxW);
        if (axisCount < 4) {
            mLabelW->setVisible(false);
            mSpinBoxW->setVisible(false);
        }

        connect(mSpinBoxX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VectorSpinBoxBase::valueChanged);
        connect(mSpinBoxY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VectorSpinBoxBase::valueChanged);
        connect(mSpinBoxZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VectorSpinBoxBase::valueChanged);
        connect(mSpinBoxW, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &VectorSpinBoxBase::valueChanged);
    }
};


// ========================================================================== //


template<typename T>
class VectorSpinBox final : public VectorSpinBoxBase
{
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

