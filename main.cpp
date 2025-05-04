#include <iostream>

#include <boost/program_options.hpp>

#include "Params.hpp"
#include "AlsaHandler.hpp"

// #define SAMPLE_RATE     8000
// #define CHANNELS        1
// #define FORMAT          SND_PCM_FORMAT_S16_LE
// #define FRAMES_PER_BUF  128

// volatile sig_atomic_t keep_running = 1;

namespace
{
const uint32_t g_sampleRate {8000};
const uint32_t g_channels {1};
const uint32_t g_latencyMax {2048};
const uint32_t g_latencyMin {32};
}

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    Params params;

    try
    {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "print this message")
            ("input,i", po::value<std::string>(&params.InputDevice), "input device")
            ("output,o", po::value<std::string>(&params.OutputDevice), "output device")
            ("format,f", po::value<std::string>(&params.SampleFormat)->default_value("S16_LE"), "sample format")
            ("rate,r", po::value<uint32_t>(&params.SampleRate)->default_value(g_sampleRate), "sample rate")
            ("channels,c", po::value<uint32_t>(&params.Channels)->default_value(g_channels), "number of channels")
            ("latency-max,M", po::value<uint32_t>(&params.LatencyMax)->default_value(g_latencyMax), "latency max")
            ("latency-min,m", po::value<uint32_t>(&params.LatencyMin)->default_value(g_latencyMin), "latency min")
            ("write,w", po::value<bool>(&params.WriteToFile)->default_value(false), "redirect to file with name of output device name")
            ("verbose,v", po::value<bool>(&params.Verbose)->default_value(false)->implicit_value(true), "print verbose logs")
            ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help"))
        {
            std::cout << desc << "\n";
            return 0;
        }

        if ((!vm.count("i") && !vm.count("input")) || (!vm.count("o") && !vm.count("output")))
            throw std::runtime_error("Input/Ouput devices must be specified");

        if ((vm.count("M") || vm.count("latency-max")) && params.LatencyMax < params.LatencyMin)
            params.LatencyMax = params.LatencyMin;

        if ((vm.count("m") || vm.count("latency-min")) && params.LatencyMax < params.LatencyMin)
            params.LatencyMin = params.LatencyMax;

        if (params.Verbose)
            std::cout << params << "\n";

        AlsaHandler alsaHandler {std::move(params)};
        alsaHandler.run();
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error occured: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}
