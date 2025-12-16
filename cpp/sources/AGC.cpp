#include "AGC.hpp"

#include <cmath>

namespace
{
const auto g_computeFrameSize = [](uint32_t rate, uint32_t lenMs) -> uint32_t {
    const auto frameSize = static_cast<uint32_t>(std::round(rate * (lenMs / 1000.0)));
    return (frameSize + (frameSize % 2));
};
}

namespace valetron::agc::lib
{

AutomaticGainControl::AutomaticGainControl(uint32_t frameLenMs, uint32_t channels, uint32_t rate,
                                           uint32_t filterSize, double peak, double gain)
    : m_channels(channels)
    , m_rate(rate)
    , m_filterSize(filterSize)
    , m_peak(peak)
    , m_maxGain(gain)
    , m_frameLen(g_computeFrameSize(rate, frameLenMs))
    , m_prefillLen(static_cast<uint32_t>(m_filterSize / 2))
{
}

} // namespace valetron::agc::lib
