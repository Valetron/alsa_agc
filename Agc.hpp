#ifndef AGC_HPP
#define AGC_HPP

#include <cmath>
#include <vector>
#include <limits>

#include "module.hpp"

namespace
{
struct AgcSettings
{
    float TargetLevel;
    float AlphaLevel;
    float BetaGain;
    float GainMax;
    float GainMin;
    float CurrentLevel;
    float CurrentGain;
};
}

class Agc : public ModuleBase
{
public:
    Agc(float target, float alpha, float beta, float maxGain, float minGain);
    ~Agc() = default;

    template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    void processFrame(std::vector<T>& buffer)
    {
        const auto bufferSize = buffer.size();
        std::vector<float> data(bufferSize, 0.0f);

        for (auto i = 0; i < bufferSize; ++i)
            data.at(i) = buffer.at(i) / std::numeric_limits<T>::max();

        gain(data);

        for (auto i = 0; i < bufferSize; ++i)
            buffer.at(i) = (std::fminf(std::fmaxf(data.at(i), -1.0f), 1.0f) * std::numeric_limits<T>::max());
    }

private:
    void gain(std::vector<float>& buffer);

private:
    AgcSettings m_agcSettings {};
};

#endif // AGC_HPP
