#include "GaussianFilter.hpp"

#include <cmath>
#include <numbers>

#include <spdlog/spdlog.h>
#include <fmt/ranges.h>

namespace
{
constexpr auto s_eps {1.0 / 3.0};
const auto s_sqrt2Pi {std::sqrt(2.0 * std::numbers::pi)};
}

namespace valetron::agc::lib
{

GaussianFilter::GaussianFilter(uint32_t filterSize)
{
    const auto sigma = (((static_cast<double>(filterSize) / 2.0) - 1.0) / 3.0) + s_eps;
    m_weights.resize(filterSize);

    double totalWeight {0.0};
    const auto offset {static_cast<uint32_t>(filterSize / 2)};
    const auto coef1 {1.0 / (sigma * s_sqrt2Pi)};
    const auto coef2 {2.0 * std::pow(sigma, 2)};

    int32_t x {0};
    for (size_t i {0}; i < filterSize; ++i)
    {
        x = i - offset;
        m_weights[i] = coef1 * std::exp(-(std::pow(x, 2) / coef2));
        totalWeight += m_weights[i];
    }

    const auto adjust {1.0 / totalWeight};
    for (size_t i {0}; i < filterSize; ++i)
        m_weights[i] *= adjust;

    SPDLOG_DEBUG("{}: sigma={}, offset={}, coef1={}, coef2={}, m_weights=[{}]",
                 __FUNCTION__, sigma, offset, coef1, coef2, fmt::join(m_weights, " "));
}

double GaussianFilter::filter(const std::deque<double>& frame) const
{
    double res {0.0};
    size_t i {0};

    for (auto iter = frame.cbegin(); iter != frame.cend(); ++iter)
        res += ((*iter) * m_weights[i++]);

    return res;
}

} // namespace valetron::agc::lib
