#ifndef AGC_ALSA_PLUGIN_H
#define AGC_ALSA_PLUGIN_H

#include <cstdint>

#include <Constants.hpp>

namespace valetron::agc::alsa_plugin
{

struct PluginParams
{
    uint32_t FrameLenMs {lib::constants::DefaultFrameLenMsec};
    uint32_t Channels {lib::constants::DefaultChannel};
    uint32_t Rate {lib::constants::DefaultSamplerate};
    uint32_t FilterSize {lib::constants::DefaultFilterSize};
    double Peak {lib::constants::DefaultPeakValue};
    double Gain {lib::constants::DefaultMaxGain};
    double Rms {lib::constants::DefaultRms};
};

} // valetron::agc::alsa_plugin

#endif // AGC_ALSA_PLUGIN_H
