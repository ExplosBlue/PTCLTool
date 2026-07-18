#pragma once

#include <QComboBox>


// ========================================================================== //

template <typename T>
concept CountEnum = std::is_enum_v<T> && requires {
    T::MAX;
};

// ========================================================================== //


template <CountEnum T>
class EnumComboBox : public QComboBox {
public:
    EnumComboBox(QWidget* parent = nullptr) :
        QComboBox(parent) {
        populate();

        connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
            updateTooltip();
        });
    }

    T currentEnum() const {
        return static_cast<T>(currentIndex());
    }

    void setCurrentEnum(T value) {
        setCurrentIndex(static_cast<int>(value));
        updateTooltip();
    }

    void setEnumText(T value, const QString& text) {
        int index = static_cast<int>(value);
        if (index >= 0 && index < count()) {
            setItemText(index, text);
        }
    }

    void setEnumTooltip(T value, const QString& tooltip) {
        int index = static_cast<int>(value);
        if (index >= 0 && index < count()) {
            setItemData(index, tooltip, Qt::ToolTipRole);
        }
    }

private:
    void updateTooltip() {
        auto index = currentIndex();
        QVariant tooltip = itemData(index, Qt::ToolTipRole);
        if (tooltip.isValid()) {
            setToolTip(tooltip.toString());
        }
    }

    void populate() {
        constexpr int maxValue = static_cast<int>(T::MAX);
        for (int i = 0; i < maxValue; ++i) {
            T value = static_cast<T>(i);
            addItem(toString(value));
        }
    }
};


// ========================================================================== //
