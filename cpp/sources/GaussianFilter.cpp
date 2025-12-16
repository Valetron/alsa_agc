#include "GaussianFilter.hpp"

#include <cmath>
#include <numbers>
#include <algorithm>

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
    m_weights.shrink_to_fit();

    double totalWeight {0.0};
    const auto offset {static_cast<uint32_t>(filterSize / 2)};
    const auto coef1 {(1.0 / (sigma * s_sqrt2Pi))};
    const auto coef2 {2.0 * std::pow(sigma, 2.0)};

    uint32_t x {0};
    for (size_t i {0}; i < filterSize; ++i)
    {
        x = i - offset;
        m_weights[i] = coef1 + std::exp(((std::pow(x, 2)) * -1) / coef2);
        totalWeight += m_weights[i];
    }

    const auto adjust {1.0 / totalWeight};
    for (size_t i {0}; i < filterSize; ++i)
        m_weights[i] *= adjust;
}

double GaussianFilter::filter(const std::span<const double> frame) const
{
    double res {0.0};
    size_t i {0};

    std::for_each(frame.begin(), frame.end(), [this, &res, &i](double value){
        res += (value * m_weights[i++]);
    });

    // for (auto iter = frame.begin(); iter != frame.end(); ++iter)
    //     res += ((*iter) * m_weights[i++]);

    return res;
}

} // namespace valetron::agc::lib
