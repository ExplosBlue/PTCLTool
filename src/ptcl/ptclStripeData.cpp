#include "ptcl/ptclStripeData.h"

namespace Ptcl {

const std::array<u8, 32>& StripeData::_0() const {
    return m_0;
}

void StripeData::set_0(const std::array<u8, 32>& _0) {
    m_0 = _0;
}

void StripeData::initFromBinary(const BinStripeData& stripeData) {
    m_0 = stripeData._0;
}


} // namespace Ptcl
