#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

namespace valetron::agc::lib::constants
{

const uint32_t DefaultFrameLenMsec {20};
const uint32_t DefaultChannel {1};
const uint32_t DefaultSamplerate {8000};
const uint32_t DefaultFilterSize {13};
const double DefaultPeakValue {0.95};
const double DefaultMaxGain {100.0};
const double DefaultRms {0.0};

} // namespace valetron::agc::lib::constants

#endif // CONSTANTS_H
