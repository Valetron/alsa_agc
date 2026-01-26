#ifndef GAUSSIANFILTER_H
#define GAUSSIANFILTER_H

#include <cstdint>
#include <vector>
#include <deque>

namespace valetron::agc::lib
{

class GaussianFilter
{
public:
    explicit GaussianFilter(uint32_t filterSize);
    double filter(const std::deque<double>& frame) const;

private:
    std::vector<double> m_weights {};
};

} // namespace valetron::agc::lib

#endif // GAUSSIANFILTER_H
