#ifndef AGC_H
#define AGC_H

#include <cstdint>
#include <vector>
#include <deque>
#include <span>

namespace
{
const auto g_defaultFilterSize {13};
const auto g_defaultPeakValue {0.95};
const auto g_defaultMaxGain {100.0};
}

namespace valetron::agc::lib
{

class AutomaticGainControl
{
public:
    AutomaticGainControl(uint32_t frameLenMs, uint32_t channels, uint32_t rate,
                         uint32_t filterSize, double peak = g_defaultPeakValue, double gain = g_defaultMaxGain);
    std::vector<double> process(std::vector<double> data);

private:
    void analyzeFrame(std::span<double> frame);
    double getMaxLocalGain(std::span<const double> frame, uint32_t channel);
    double findPeakMagnitude(std::span<const double> frame, uint32_t channel);
    void updateGainHistory(uint32_t channel, double currGain);
    void dcCorrection(std::span<double> frame);
    void amplify(std::span<double> frame);

private:
    const uint32_t m_channels {};
    const uint32_t m_rate {};
    const uint32_t m_filterSize {};
    const double m_peak {};
    const double m_maxGain {};
    const uint32_t m_frameLen {};
    const uint32_t m_prefillLen {};
    std::deque<double> m_gainHistory_original {};
    std::deque<double> m_gainHistory_minimum {};
    std::deque<double> m_gainHistory_smoothed {};


    // NOTE: rms, compress
};

}

#endif // AGC_H
