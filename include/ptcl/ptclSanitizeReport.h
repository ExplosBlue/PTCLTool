#pragma once

#include "typedefs.h"

#include <QString>
#include <QStringList>

#include <type_traits>
#include <utility>


namespace Ptcl {


// ========================================================================== //


namespace detail {

template<typename T>
constexpr s64 rawValue(T value) {
    if constexpr (std::is_enum_v<T>) {
        return static_cast<s64>(std::to_underlying(value));
    }
    return static_cast<s64>(value);
}

} // namespace detail


// ========================================================================== //


template<typename EnumType>
struct SanitizedValue {
    EnumType value{};
    bool wasInvalid{false};
    QString issue{};
};


// ========================================================================== //


template<typename EnumType, typename RawType>
SanitizedValue<EnumType> sanitizeEnum(RawType raw, EnumType maxValue, EnumType fallback, const QString& fieldName) {
    static_assert(std::is_enum_v<EnumType>, "sanitizeEnum requires an enum EnumType");
    using UnderlyingType = std::underlying_type_t<EnumType>;

    const s64 value = detail::rawValue(raw);
    const s64 max = detail::rawValue(maxValue);

    if (value >= 0 && value <= max) {
        return {static_cast<EnumType>(static_cast<UnderlyingType>(value)), false, {}};
    }

    SanitizedValue<EnumType> result;
    result.value = fallback;
    result.wasInvalid = true;
    result.issue = QStringLiteral("field '%1' has invalid value %2; clamped to %3.")
       .arg(fieldName)
       .arg(value)
       .arg(detail::rawValue(fallback));
    return result;
}

template<typename EnumType, typename RawType>
SanitizedValue<EnumType> sanitizeEnum(RawType raw, EnumType maxEnum, QString fieldName) {
    return sanitizeEnum(raw, maxEnum, maxEnum, fieldName);
}


// ========================================================================== //


class PtclSanitizeReport {
public:
    bool hasIssues() const { return !mIssues.isEmpty(); }
    s32 count() const { return static_cast<s32>(mIssues.size()); }
    const QStringList& issues() const { return mIssues; }

    void setContext(QString context) { mContext = std::move(context); }
    const QString& context() const { return mContext; }

    void add(QString issue) { mIssues.push_back(std::move(issue)); }

    template<typename EnumType, typename RawType>
    EnumType sanitize(RawType raw, EnumType maxValue, QString fieldName) {
        return sanitize(raw, maxValue, maxValue, fieldName);
    }

    template<typename EnumType, typename RawType>
    EnumType sanitize(RawType raw, EnumType maxValue, EnumType fallback, QString fieldName) {
        const auto result = sanitizeEnum(raw, maxValue, fallback, fieldName);
        if (!result.wasInvalid) {
            return result.value;
        }

        add(mContext.isEmpty()
            ? result.issue
            : mContext + QStringLiteral(": ") + result.issue);

        return result.value;
    }

private:
    QStringList mIssues;
    QString mContext;
};


// ========================================================================== //


} // namespace Ptcl
