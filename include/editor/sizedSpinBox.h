#pragma once

#include "typedefs.h"

#include <QAbstractSpinBox>
#include <QSpinBox>
#include <QKeyEvent>
#include <QLineEdit>
#include <QStyleOption>


// ========================================================================== //


class SizedSpinBoxBase : public QAbstractSpinBox {
    Q_OBJECT
public:
    explicit SizedSpinBoxBase(QWidget* parent = nullptr) :
        QAbstractSpinBox{parent} {}

signals:
    void valueChanged(u64 value);
};


// ========================================================================== //


template<typename T>
class SizedSpinBox final : public SizedSpinBoxBase {
    static_assert(std::is_integral_v<T>, "SizedSpinBox only supports integral types");

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

    void setRange(T minValue, T maxValue) {
        constexpr T typeMin = std::numeric_limits<T>::min();
        constexpr T typeMax = std::numeric_limits<T>::max();

        if (minValue < typeMin) {
            qWarning() << "setRange: minValue" << minValue << "is less than type minimum" << typeMin << ". Clamping to type minimum.";
        }
        if (maxValue > typeMax) {
            qWarning() << "setRange: maxValue" << maxValue << "is greater than type maximum" << typeMax << ". Clamping to type maximum.";
        }
        if (minValue > maxValue) {
            qWarning() << "setRange: minValue" << minValue << "is greater than maxValue" << maxValue << ". Adjusting maxValue to minValue.";
        }

        mMin = std::clamp(minValue, typeMin, typeMax);
        mMax = std::clamp(maxValue, mMin, typeMax);

        setValue(mCurrentValue);
    }

    void setMinimum(T minValue) {
        setRange(minValue, mMax);
    }

    void setMaximum(T maxValue) {
        setRange(mMin, maxValue);
    }

    T minimum() const {
        return mMin;
    }

    T maximum() const {
        return mMax;
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

        if constexpr (std::is_unsigned_v<T>) {
            value = static_cast<T>(input.toULongLong(&ok));
        } else {
            value = static_cast<T>(input.toLongLong(&ok));
        }

        if (ok && value >= mMin && value <= mMax) {
            return QValidator::Acceptable;
        }
        return QValidator::Invalid;
    }

    void interpretText() {
        bool ok = false;
        T value;

        QString text = lineEdit()->text();

        if constexpr (std::is_unsigned_v<T>) {
            value = static_cast<T>(text.toULongLong(&ok));
        } else {
            value = static_cast<T>(text.toLongLong(&ok));
        }

        if (ok && value >= mMin && value <= mMax) {
            if (value != mCurrentValue) {
                setValue(value);
            }
        } else {
            updateDisplay();
        }
    }

    void focusOutEvent(QFocusEvent* event) final {
        interpretText();
        QAbstractSpinBox::focusOutEvent(event);
    }

    void keyPressEvent(QKeyEvent* event) final {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            interpretText();
        }
        QAbstractSpinBox::keyPressEvent(event);
    }

    QSize sizeHint() const override {
        static QSpinBox ref;
        ref.setFont(font());
        return ref.sizeHint();
    }

    QSize minimumSizeHint() const override {
        return sizeHint();
    }

protected:
    void updateDisplay()  {
        lineEdit()->setText(QString::number(mCurrentValue));
    }

private:
    T mCurrentValue;
    T mMin;
    T mMax;
};


// ========================================================================== //
