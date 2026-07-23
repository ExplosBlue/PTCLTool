#pragma once

#include <QComboBox>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>

#include <array>
#include <functional>
#include <span>
#include <type_traits>


// ========================================================================== //


template<typename T>
struct EnumOption {
    T value;
    QString name;
    QString description;
};


// ========================================================================== //


template <typename T>
class EnumFilterProxyModel : public QSortFilterProxyModel {
public:
    using FilterFn = std::function<bool(T)>;

    explicit EnumFilterProxyModel(QObject* parent = nullptr) :
        QSortFilterProxyModel{parent} {}

    void setFilterFn(FilterFn filter) {
        beginFilterChange();
        mFilter = std::move(filter);
        endFilterChange();
    }

    void clearFilterFn() {
        beginFilterChange();
        mFilter = nullptr;
        endFilterChange();
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override {
        if (!mFilter) {
            return true;
        }

        const QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        const T value = index.data(Qt::UserRole).template value<T>();
        return mFilter(value);
    }

private:
    FilterFn mFilter;
};


// ========================================================================== //


template <typename T>
class EnumComboBox : public QComboBox {
    static_assert(std::is_enum_v<T>, "EnumComboBox requires an enum type");
public:
    using Option = EnumOption<T>;
    using OptionList = std::span<const Option>;

    explicit EnumComboBox(QWidget* parent = nullptr) :
        QComboBox{parent},
        mSourceModel{new QStandardItemModel{this}},
        mProxy{new EnumFilterProxyModel<T>{this}} {

        mProxy->setSourceModel(mSourceModel);
        setModel(mProxy);

        connect(this, &QComboBox::currentIndexChanged, this, [this](int) {
            updateToolTip();
        });
    }

    template <std::size_t N>
    void setOptions(const std::array<Option, N>& options) {
        setOptions(std::span<const Option>{options});
    }

    void setDescription(const QString& description) {
        mDescription = description;
        updateToolTip();
    }

    void setOptions(OptionList options) {
        const T prev = currentEnum();

        mSourceModel->clear();

        for (const auto& option : options) {
            auto* item = new QStandardItem();
            item->setText(option.name);
            item->setData(QVariant::fromValue(option.value), Qt::UserRole);
            item->setToolTip(option.description);
            item->setEditable(false);
            mSourceModel->appendRow(item);
        }

        setCurrentEnum(prev);
        updateToolTip();
    }

    T currentEnum() const {
        const int idx = currentIndex();
        if (idx < 0) {
            return T{};
        }

        const QModelIndex proxyIndex = mProxy->index(idx, 0);
        const QModelIndex sourceIndex = mProxy->mapToSource(proxyIndex);
        return mSourceModel->data(sourceIndex, Qt::UserRole).value<T>();
    }

    void setCurrentEnum(T value) {
        for (int i = 0; i < mSourceModel->rowCount(); ++i) {
            const QModelIndex sourceIndex = mSourceModel->index(i, 0);
            const T itemValue = mSourceModel->data(sourceIndex, Qt::UserRole).value<T>();

            if (itemValue == value) {
                const QModelIndex proxyIndex = mProxy->mapFromSource(sourceIndex);
                if (proxyIndex.isValid()) {
                    setCurrentIndex(proxyIndex.row());
                }
                updateToolTip();
                return;
            }
        }
    }

    void setFilter(std::function<bool(T)> filter) {
        mProxy->setFilterFn(std::move(filter));
        updateToolTip();
    }

    void clearFilter() {
        mProxy->clearFilterFn();
        updateToolTip();
    }

    void invalidateFilter() {
        mProxy->invalidateFilter();
        updateToolTip();
    }

private:
    void updateToolTip() {
        QString tooltip;

        if (!mDescription.isEmpty()) {
            tooltip += mDescription.toHtmlEscaped();
        }

        const QVariant tooltipData = currentData(Qt::ToolTipRole);
        if (tooltipData.isValid()) {
            if (!tooltip.isEmpty()) {
                tooltip += "<br><br>";
            }

            const QString optionName = currentText().toHtmlEscaped();
            const QString optionDesc = tooltipData.toString().toHtmlEscaped();

            tooltip += QString("<b>%1</b><br>%2")
                .arg(optionName, optionDesc);
        }

        setToolTip(tooltip);
    }

private:
    QStandardItemModel* mSourceModel;
    EnumFilterProxyModel<T>* mProxy;
    QString mDescription;
};


// ========================================================================== //
