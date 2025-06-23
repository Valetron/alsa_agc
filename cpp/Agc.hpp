#ifndef AGC_HPP
#define AGC_HPP

#include <iostream>
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
    // Agc();
    // ~Agc() = default;

    void process(uint16_t* data, int size);

    // template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
    // void processFrame(std::vector<T>& buffer, int size)
    // {
    //     // std::clog << "size = " << buffer.size() << "\n";
    //     std::vector<double> res(size, 0);
    //     std::string str {};
    //     for (auto i = 0; i < size; ++i)
    //     {
    //         res.at(i) = buffer.at(i) / std::numeric_limits<T>::max();
    //         str.append(std::to_string(res.at(i))).append(" ");
    //         // buffer.at(i) *= std::numeric_limits<T>::max();
    //     }
    //     std::clog << "data: " << str << "\n";
    // }

private:
    void gain(std::vector<float>& buffer);

private:
    AgcSettings m_agcSettings {};
};

#endif // AGC_HPP
