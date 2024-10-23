#ifndef SIZEDSPINBOX_H
#define SIZEDSPINBOX_H

#include <QAbstractSpinBox>
#include <QLineEdit>

#include "typedefs.h"


class SizedSpinBoxBase : public QAbstractSpinBox {
    Q_OBJECT
public:
    explicit SizedSpinBoxBase(QWidget* parent = nullptr) :
        QAbstractSpinBox{parent} {}

signals:
    void valueChanged(u64 value);
};


template<typename T>
class SizedSpinBox final : public SizedSpinBoxBase {
    static_assert(std::is_integral<T>::value, "SizedSpinBox only supports integral types");

public:

    explicit SizedSpinBox(QWidget* parent = nullptr) :
        SizedSpinBoxBase{parent} {

        setRange(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        setValue(0);
    }

    T value() const {
        return static_cast<T>(mCurrentValue);
    }

    void setValue(T value) {

        mCurrentValue = std::clamp(value, mMin, mMax);
        updateDisplay();
        emit valueChanged(mCurrentValue);
    }

    void stepBy(int steps) final {

        setValue(mCurrentValue + steps);
    }

    StepEnabled stepEnabled() const final {
        StepEnabled flags = StepNone;

        if (mCurrentValue > mMin) {
            flags |= StepDownEnabled;
        }
        if (mCurrentValue < mMax) {
            flags |= StepUpEnabled;
        }
        return flags;
    }

    QValidator::State validate(QString& input, int& pos) const final {

        bool ok = false;
        T value;

        if constexpr (std::is_unsigned<T>::value) {
            value = static_cast<T>(input.toULongLong(&ok));
        } else {
            value = static_cast<T>(input.toLongLong(&ok));
        }

        if (ok && value >= mMin && value <= mMax) {
            return QValidator::Acceptable;
        }
        return QValidator::Invalid;
    }

protected:
    void updateDisplay()  {

        lineEdit()->setText(QString::number(mCurrentValue));
    }

private:
    void setRange(T minValue, T maxValue) {

        mMin = minValue;
        mMax = maxValue;
    }

private:
    T mCurrentValue;
    T mMin;
    T mMax;
};


#endif // SIZEDSPINBOX_H
