#include "AGC.hpp"

#include <cmath>
#include <limits>
#include <numbers>
#include <algorithm>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

namespace
{
constexpr auto g_dblEpsilon = std::numeric_limits<double>::epsilon();
constexpr auto g_dblEpsilonMax = std::numeric_limits<double>::max();

const auto g_sqrtPi2 = (std::sqrt(std::numbers::pi) / 2.0);

uint32_t computeFrameSize(uint32_t rate, uint32_t lenMs)
{
    const auto frameSize = static_cast<uint32_t>(std::round(rate * (lenMs / 1000.0)));
    return (frameSize + (frameSize % 2));
};

inline double bound(double thr, double val)
{
    return (std::erf(g_sqrtPi2 * (val / thr)) * thr);
}

inline double updateVal(double newVal, double oldVal, double aggr)
{
    return (aggr * newVal) + ((1.0 - aggr) * oldVal);
}

}

namespace valetron::agc::lib
{

AutomaticGainControl::AutomaticGainControl(uint32_t frameLenMs, uint32_t channels, uint32_t rate,
                                           uint32_t filterSize, double peak, double gain, double targetRms)
    : m_channels(channels)
    , m_rate(rate)
    , m_filterSize(filterSize)
    , m_peak(peak)
    , m_maxGain(gain)
    , m_targetRms(targetRms)
    , m_frameLen(computeFrameSize(rate, frameLenMs))
    , m_prefillLen(static_cast<uint32_t>(m_filterSize / 2))
{
    m_gauss = std::make_unique<GaussianFilter>(m_filterSize);

    for (size_t i {0}; i < m_channels; ++i)
    {
        m_gainHistoryOriginal.emplace_back(std::deque<double>());
        m_gainHistoryMinimum.emplace_back(std::deque<double>());
        m_gainHistorySmoothed.emplace_back(std::deque<double>());
    }

    m_dcCorrectionValue = std::deque<double>(m_channels, 0.0);
    m_prevAmplificationFactor = std::deque<double>(m_channels, 0.0);

    std::fill(m_fadeFactors.begin(), m_fadeFactors.end(), std::vector<double>(m_frameLen, 0.0));
    precalculateFadeFactors();
}

std::vector<double> AutomaticGainControl::process(std::vector<double> rawData)
{
    auto processedData {std::move(rawData)};

    analyzeFrame(processedData);
    m_window.emplace_back(std::move(processedData));

    if (m_window.size() >= m_filterSize && !m_gainHistorySmoothed[0].empty())
    {
        auto resFrame = m_window.front();
        m_window.pop_front();
        amplify(resFrame);
        return resFrame;
    }

    return {};
}

uint32_t AutomaticGainControl::getFrameLenBytes() const
{
    return m_frameLen;
}

void AutomaticGainControl::analyzeFrame(std::span<double> frame)
{
    dcCorrection(frame);
    double maxLocalGain {0.0};
    for (size_t ch {0}; ch < m_channels; ++ch)
    {
        maxLocalGain = getMaxLocalGain(frame);
        updateGainHistory(ch, maxLocalGain);
    }
}

double AutomaticGainControl::getMaxLocalGain(const std::span<const double> frame) const
{
    const auto maxGain = m_peak / findPeakMagnitude(frame);
    const auto rmsGain = (m_targetRms > g_dblEpsilon) ? (m_targetRms / computeFrameRms(frame)) : g_dblEpsilonMax;
    return bound(m_maxGain, std::min(maxGain, rmsGain));
}

double AutomaticGainControl::findPeakMagnitude(const std::span<const double> frame/*, uint32_t channel*/) const
{
    auto res {g_dblEpsilon};
    for (auto iter = frame.begin(); iter != frame.end(); ++iter)
        res = std::max(res, std::fabs(*iter));

    return res;
}

double AutomaticGainControl::computeFrameRms([[maybe_unused]] const std::span<const double> frame) const
{
    return 0.0; // ((((-:
}

inline double AutomaticGainControl::fade(double prevVal, double nextVal, uint32_t pos) const
{
    return ((m_fadeFactors[0][pos] * prevVal) + (m_fadeFactors[1][pos] * nextVal));
}

void AutomaticGainControl::updateGainHistory(uint32_t channel, double currGain)
{
    if (m_gainHistoryOriginal[channel].empty() || m_gainHistoryMinimum[channel].empty())
    {
        const auto initVal {1.0};
        m_prevAmplificationFactor[channel] = initVal;
        while (m_gainHistoryOriginal[channel].size() < m_prefillLen)
            m_gainHistoryOriginal[channel].push_back(initVal);
    }

    m_gainHistoryOriginal[channel].push_back(currGain);

    while (m_gainHistoryOriginal[channel].size() >= m_filterSize)
    {
        if (m_gainHistoryMinimum[channel].empty())
        {
            auto initVal {1.0};
            auto iter = m_gainHistoryOriginal[channel].begin() + m_prefillLen;
            while (m_gainHistoryMinimum[channel].size() < m_prefillLen)
            {
                initVal = std::min(initVal, *(++iter));
                m_gainHistoryMinimum[channel].push_back(initVal);
            }
        }

        const auto minElemIter = std::min_element(m_gainHistoryOriginal[channel].cbegin(), m_gainHistoryOriginal[channel].cend());
        const auto minElem = *minElemIter;
        m_gainHistoryOriginal[channel].pop_front();
        m_gainHistoryMinimum[channel].push_back(minElem);
    }

    while (m_gainHistoryMinimum[channel].size() >= m_filterSize)
    {
        const auto smoothed = m_gauss->filter(m_gainHistoryMinimum[channel]);
        m_gainHistoryMinimum[channel].pop_front();
        m_gainHistorySmoothed[channel].push_back(smoothed);
    }
}

void AutomaticGainControl::dcCorrection(std::span<double> frame)
{
    const auto isFirstFrame = m_gainHistoryOriginal[0].empty();
    const auto diff = (1.0 / static_cast<double>(m_frameLen));
    const auto frameSize = frame.size();

    for (size_t ch {0}; ch < m_channels; ++ch)
    {
        auto currAvgVal {0.0};
        for (size_t i {0}; i < frameSize; ++i)
            currAvgVal += (frame[i] * diff);

        const auto prevVal = isFirstFrame ? currAvgVal : m_dcCorrectionValue[ch];
        m_dcCorrectionValue[ch] = isFirstFrame ? currAvgVal : updateVal(currAvgVal, m_dcCorrectionValue[ch], 0.1);

        for (size_t i {0}; i < frameSize; ++i)
            frame[i] -= fade(prevVal, m_dcCorrectionValue[ch], i);
    }
}

void AutomaticGainControl::amplify(std::span<double> frame)
{
    const auto frameSize = frame.size();
    for (size_t ch {0}; ch < m_channels; ++ch)
    {
        if (m_gainHistorySmoothed[ch].empty())
            break;

        const auto currGain = m_gainHistorySmoothed[ch].front();
        m_gainHistorySmoothed[ch].pop_front();

        double gainFactor {0.0};
        for (size_t i {0}; i < frameSize; ++i)
        {
            gainFactor = fade(m_prevAmplificationFactor[ch], currGain, i);
            frame[i] *= gainFactor;

            if (std::fabs(frame[i]) > m_peak)
                frame[i] = std::copysign(m_peak, frame[i]);
        }

        m_prevAmplificationFactor[ch] = currGain;
    }
}

void AutomaticGainControl::precalculateFadeFactors()
{
    const double dStepSize = 1.0 / static_cast<double>(m_frameLen);
    for (size_t i {0}; i < m_frameLen; ++i)
    {
        m_fadeFactors[0][i] = (1.0 - (dStepSize * static_cast<double>(i + 1)));
        m_fadeFactors[1][i] = (1.0 - m_fadeFactors[0][i]);
    }
}

bool AutomaticGainControl::isDataLeft() const
{
    return !m_window.empty();
}

std::vector<double> AutomaticGainControl::getFrame()
{
    auto resFrame = m_window.front();
    m_window.pop_front();
    return resFrame;
}

} // namespace valetron::agc::lib
