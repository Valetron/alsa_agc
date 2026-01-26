#include <limits>
#include <iostream>
#include <algorithm>

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ranges.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <boost/program_options.hpp>

#include "Constants.hpp"
#include "AGC.hpp"

using namespace valetron::agc::lib;
using namespace valetron::agc::lib::constants;
namespace bpo = boost::program_options;

namespace
{
constexpr auto g_maxByte {std::numeric_limits<uint8_t>::max()};
constexpr auto g_sampleFormat_S16_LE {sizeof(int16_t)};
constexpr auto g_maxVal {std::numeric_limits<int16_t>::max()};
constexpr auto g_minShort {std::numeric_limits<int16_t>::min()};

void configureLogger()
{
    spdlog::set_pattern("[%H:%M:%S.%e](%l): %v");
    auto stderr_logger = spdlog::stderr_color_mt("stderr_logger");
    spdlog::set_default_logger(stderr_logger);
}

void configureIO()
{
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
}
}

int main(int argc, char** argv)
{
    try
    {
        configureLogger();

        auto frameLenMsec {DefaultFrameLenMsec};
        auto channel {DefaultChannel};
        auto rate {DefaultSamplerate};
        auto filter {DefaultFilterSize};
        auto peak {DefaultPeakValue};
        auto gain {DefaultMaxGain};

        bpo::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "Print help")
            ("msec,m", bpo::value<decltype(frameLenMsec)>(&frameLenMsec)->default_value(frameLenMsec), "Frame length in msec")
            ("channel,c", bpo::value<decltype(channel)>(&channel)->default_value(channel), "Channels to filter")
            ("rate,r", bpo::value<decltype(rate)>(&rate)->default_value(rate), "Sample rate")
            ("filter,f", bpo::value<decltype(filter)>(&filter)->default_value(filter), "Gaussian filter size")
            ("peak,p", bpo::value<decltype(peak)>(&peak)->default_value(peak, std::to_string(peak)), "Max peak value")
            ("gain,g", bpo::value<decltype(gain)>(&gain)->default_value(gain), "Max gain value")
            ("verbose,v", bpo::bool_switch(), "Enable verbose logs");

        bpo::variables_map vm;
        bpo::store(bpo::parse_command_line(argc, argv, desc), vm);

        if (vm.count("help") || argc < 1)
        {
            desc.print(std::cerr, 0);
            return 0;
        }

        bpo::notify(vm);

        const auto logLevel = vm["verbose"].as<bool>() ? spdlog::level::debug : spdlog::level::info;
        spdlog::set_level(logLevel);

        AutomaticGainControl agc {frameLenMsec, channel, rate, filter, peak, gain};

        configureIO();

        const auto bytesToRead = agc.getFrameLenBytes() * g_sampleFormat_S16_LE;

        std::vector<uint8_t> inputBuf(bytesToRead);
        std::vector<double> normalized(bytesToRead / g_sampleFormat_S16_LE);
        size_t count {0};

        while (std::cin.read(reinterpret_cast<char*>(inputBuf.data()), bytesToRead) || std::cin.gcount() > 0)
        {
            const auto readBytes = std::cin.gcount();

            if (readBytes > bytesToRead)
                throw std::runtime_error(fmt::format("Excpect {} bytes, actually read {}", bytesToRead, readBytes));

            std::vector<int16_t> tmp;
            tmp.reserve(bytesToRead / g_sampleFormat_S16_LE);
            for (auto i {0}; i < bytesToRead; i += 2)
            {
                const int16_t val = static_cast<int16_t>(inputBuf[i]) | (static_cast<int16_t>(inputBuf[i + 1]) << 8);
                tmp.push_back(val);
            }

            std::transform(tmp.cbegin(), tmp.cend(),
                           normalized.begin(),
                           [](auto byte) { return static_cast<double>(byte) / g_maxVal; });

            auto res = agc.process(normalized);
            if (res.empty())
                continue;

            std::vector<int16_t> out {};
            out.reserve(res.size());

            for (const auto num : res)
            {
                const auto val = num * g_maxVal;
                const int16_t clamped = std::clamp(static_cast<int16_t>(val), g_minShort, g_maxVal);
                out.push_back(clamped);
            }

            SPDLOG_TRACE("out=[{}]", fmt::join(out, " "));

            std::cout.write(reinterpret_cast<char*>(out.data()), out.size() * g_sampleFormat_S16_LE);
            std::cout.flush();
            ++count;
        }

        while (agc.isDataLeft())
        {
            auto res = agc.getFrame();
            std::vector<int16_t> out {};
            out.reserve(res.size());

            for (const auto num : res)
            {
                const auto val = num * g_maxVal;
                const int16_t clamped = std::clamp(static_cast<int16_t>(val), g_minShort, g_maxVal);
                out.push_back(clamped);
            }

            std::cout.write(reinterpret_cast<char*>(out.data()), out.size() * g_sampleFormat_S16_LE);
            std::cout.flush();
            ++count;
        }

        SPDLOG_INFO("Processed {} frames", count);
    }
    catch (const bpo::error& ex)
    {
        SPDLOG_ERROR("Failed to parse args: {}", ex.what());
        return 42;
    }
    catch (const std::exception& ex)
    {
        SPDLOG_ERROR(ex.what());
        return 42;
    }

    return 0;
}
