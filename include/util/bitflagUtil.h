#pragma once

#include <QDataStream>

#include <type_traits>


// ========================================================================== //


template <typename T>
concept ScopedEnum = std::is_enum_v<T> && !std::is_convertible_v<T, int>;

template <ScopedEnum T>
class BitFlag
{
private:
    using BaseType = std::underlying_type_t<T>;

public:
    constexpr BitFlag() = default;

    constexpr explicit BitFlag(T flag) :
        mFlag(static_cast<BaseType>(flag)) {}

    constexpr explicit BitFlag(BaseType raw) :
        mFlag(raw) {}

    constexpr BitFlag(std::initializer_list<T> flags) {
        for (T f : flags) {
            enable(f);
        }
    };

    void enable(T flag) {
        mFlag |= static_cast<BaseType>(flag);
    }

    void clear(T flag) {
        mFlag &= ~static_cast<BaseType>(flag);
    };

    void set(T flag, bool enabled) {
        return enabled ? enable(flag) : clear(flag);
    }

    void reset() {
        mFlag = 0;
    };

    bool isSet(T flag) const {
        return (mFlag & static_cast<BaseType>(flag)) != 0;
    };

    bool any() const {
        return mFlag == 0;
    }

    bool all(BitFlag other) const {
        return (mFlag & other.mFlag) == other.mFlag;
    }

    bool any(BitFlag other) const {
        return (mFlag & other.mFlag) != 0;
    }

    BitFlag& operator |=(BitFlag rhs) {
        mFlag |= rhs.mFlag;
    }

    BitFlag& operator &=(BitFlag rhs) {
        mFlag &= rhs.mFlag;
    }

    friend BitFlag operator|(BitFlag lhs, BitFlag rhs) {
        lhs |= rhs;
        return lhs;
    }

    friend BitFlag operator&(BitFlag lhs, BitFlag rhs) {
        lhs &= rhs;
        return lhs;
    }

    friend bool operator==(BitFlag lhs, BitFlag rhs) {
        return lhs.mFlag == rhs.mFlag;
    }

    friend bool operator!=(BitFlag lhs, BitFlag rhs) {
        return lhs.mFlag != rhs.mFlag;
    }

    explicit operator bool() const {
        return any();
    }

    BaseType value() const {
        return mFlag;
    }

    friend QDataStream& operator>>(QDataStream& in, BitFlag<T>& bitflag) {
        BaseType data;
        in >> data;
        bitflag.mFlag = data;
        return in;
    }

    friend QDataStream& operator<<(QDataStream& out, const BitFlag<T>& bitflag) {
        out << bitflag.mFlag;
        return out;
    }

    friend QDebug& operator<<(QDebug& dbg, const BitFlag<T>& bitflag) {
        dbg << bitflag.mFlag;
        return dbg;
    }

private:
    BaseType mFlag{};
};


// ========================================================================== //
