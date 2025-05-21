#ifndef ENUMCOMBOBOX_H
#define ENUMCOMBOBOX_H

#include <QComboBox>

template <typename T>
class EnumComboBox : public QComboBox {
    static_assert(std::is_enum<T>::value, "EnumComboBox requires an enum type");

public:
    EnumComboBox(QWidget* parent = nullptr) :
        QComboBox(parent) {
        populate();
    }

    T currentEnum() const {
        return static_cast<T>(currentIndex());
    }

    void setCurrentEnum(T value) {
        setCurrentIndex(static_cast<int>(value));
    }

private:
    void populate() {
        constexpr int maxValue = static_cast<int>(T::MAX);
        for (int i = 0; i < maxValue; ++i) {
            T value = static_cast<T>(i);
            addItem(toString(value));
        }
    }
};


#endif // ENUMCOMBOBOX_H
