#include "Agc.hpp"

namespace
{
constexpr float g_epsilon = 1e-5f;
}

Agc::Agc(float target, float alpha, float beta, float maxGain, float minGain)
{
    m_agcSettings.TargetLevel = target;
    m_agcSettings.AlphaLevel = alpha;
    m_agcSettings.BetaGain = beta;
    m_agcSettings.GainMax = maxGain;
    m_agcSettings.GainMin = minGain;
    m_agcSettings.CurrentLevel = 0.0f;
    m_agcSettings.CurrentGain = 1.0f;
}

void Agc::gain(std::vector<float>& buffer)
{
    for (auto i = 0; i < buffer.size(); ++i)
    {
        auto sample = buffer.at(i);
        float absSample = fabsf(sample);
        m_agcSettings.CurrentLevel = m_agcSettings.AlphaLevel * absSample
                                     + (1.0f - m_agcSettings.AlphaLevel) * m_agcSettings.CurrentLevel;
        float targetGain = m_agcSettings.TargetLevel / (m_agcSettings.CurrentLevel + g_epsilon);
        float smoothedGain = m_agcSettings.BetaGain * m_agcSettings.CurrentGain + (1.0f - m_agcSettings.BetaGain) * targetGain;
        m_agcSettings.CurrentGain = std::fminf(std::fmaxf(smoothedGain, m_agcSettings.GainMin), m_agcSettings.GainMax);
        sample *= m_agcSettings.CurrentGain;
    }
}
