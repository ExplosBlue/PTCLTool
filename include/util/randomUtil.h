#pragma once

#include <random>


namespace Random {


// ========================================================================== //


inline std::mt19937& rng() {
    static std::mt19937 sRng{[] {
            std::random_device rd;
            std::seed_seq seq{
                rd(), rd(), rd(), rd(),
                rd(), rd(), rd(), rd()
            };
            return std::mt19937{seq};
    }()};

    return sRng;
}

template<typename T>
inline T randomInt(T min, T max) {
    static_assert(std::is_integral_v<T>, "randomInt requires an integral type");
    return std::uniform_int_distribution<T>{min, max}(rng());
}


// ========================================================================== //


} // namespace Random
