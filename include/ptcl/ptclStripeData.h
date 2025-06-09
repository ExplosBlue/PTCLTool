#pragma once

#include "ptcl/ptclBinary.h"
#include "typedefs.h"

#include <array>


namespace Ptcl {


// ========================================================================== //


class StripeData
{
public:
    StripeData() = default;

    StripeData(const StripeData&) = delete;
    StripeData& operator=(const StripeData&) = delete;

    const std::array<u8, 32>& _0() const;
    void set_0(const std::array<u8, 32>& _0);

    void initFromBinary(const BinStripeData& stripeData);

private:
    std::array<u8, 32> m_0;
};


// ========================================================================== //


} // namespace Ptcl
