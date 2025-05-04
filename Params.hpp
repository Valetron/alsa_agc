#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>
#include <ostream>

struct Params final
{
    std::string InputDevice {};
    std::string OutputDevice {"default"};
    std::string SampleFormat {"S16_LE"};
    uint32_t SampleRate {8000};
    uint32_t Channels {1};
    uint32_t LatencyMax {2048};
    uint32_t LatencyMin {32};
    bool WriteToFile {false};
    bool Verbose {false};

    friend std::ostream& operator << (std::ostream& os, const Params& obj)
    {
        return os << "Parameters\n"
                  << "Input device:\t"  << obj.InputDevice << "\n"
                  << "Output device:\t" << obj.OutputDevice << "\n"
                  << "Sample format:\t" << obj.SampleFormat << "\n"
                  << "Sample rate:\t"   << obj.SampleRate << "\n"
                  << "Channels:\t"      << obj.Channels << "\n"
                  << "Latency max:\t"   << obj.LatencyMax << "\n"
                  << "Latency min:\t"   << obj.LatencyMin << "\n"
                  << "Write to file:\t" << (obj.WriteToFile ? obj.OutputDevice : "No");
    }
};


#endif // PARAMS_HPP
