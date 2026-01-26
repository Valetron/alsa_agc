#ifndef AGC_H
#define AGC_H

#include <cstdint>
#include <memory>
#include <vector>
#include <deque>
#include <span>
#include <array>

#include "GaussianFilter.hpp"
#include "Constants.hpp"

namespace valetron::agc::lib
{

using TGainHistory = std::vector<std::deque<double>>;

class AutomaticGainControl
{
public:
    AutomaticGainControl(uint32_t frameLenMs, uint32_t channels, uint32_t rate,
                         uint32_t filterSize, double peak = constants::DefaultPeakValue, double gain = constants::DefaultMaxGain,
                         double targetRms = constants::DefaultRms);
    std::vector<double> process(std::vector<double> data);
    uint32_t getFrameLenBytes() const;
    bool isDataLeft() const;
    std::vector<double> getFrame();

private:
    void analyzeFrame(std::span<double> frame);
    double getMaxLocalGain(const std::span<const double> frame) const;
    double findPeakMagnitude(const std::span<const double> frame) const;
    double computeFrameRms(const std::span<const double> frame) const;
    inline double fade(double prevVal, double nextVal, uint32_t pos) const;
    void updateGainHistory(uint32_t channel, double currGain);
    void dcCorrection(std::span<double> frame);
    void amplify(std::span<double> frame);
    void precalculateFadeFactors();

private:
    const uint32_t m_channels {};
    const uint32_t m_rate {};
    const uint32_t m_filterSize {};
    const double m_peak {};
    const double m_maxGain {};
    const double m_targetRms {};
    const uint32_t m_frameLen {};
    const uint32_t m_prefillLen {};
    TGainHistory m_gainHistoryOriginal {};
    TGainHistory m_gainHistoryMinimum {};
    TGainHistory m_gainHistorySmoothed {};
    std::deque<double> m_dcCorrectionValue {};
    std::deque<double> m_prevAmplificationFactor {};
    std::array<std::vector<double>, 2> m_fadeFactors {};
    std::deque<std::vector<double>> m_window {};

    std::unique_ptr<GaussianFilter> m_gauss {nullptr};

    // NOTE: rms, compress
};

}

#endif // AGC_H
