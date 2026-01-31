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
    BitFlag() = default;

    explicit BitFlag(std::initializer_list<T> flags) {
        for (T f : flags) {
            enable(f);
        }
    };

    void enable(T flag) {
        mFlag |= static_cast<BaseType>(flag);
    }

    void set(T flag, bool enabled) {
        return enabled ? enable(flag) : clear(flag);
    }

    void clear(T flag) {
        mFlag &= ~static_cast<BaseType>(flag);
    };

    bool isSet(T flag) const {
        return (mFlag & static_cast<BaseType>(flag)) != 0;
    };

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
