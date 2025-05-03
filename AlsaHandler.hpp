#ifndef ALSA_HANDLER_HPP
#define ALSA_HANDLER_HPP

#include <utility>

extern "C"
{
#include <alsa/asoundlib.h>
}

#include "Params.hpp"

class AlsaHandler final
{
public:
    AlsaHandler(Params&& params);
    ~AlsaHandler();
    void run();

private:
    std::pair<bool, std::string> setHwParams(snd_pcm_t* handle);
    void close();
    // void setSwParams(snd_pcm_t* handle);

private:
    Params m_params;
    snd_pcm_t* m_capture {nullptr};
    snd_pcm_t* m_playback {nullptr};
};

#endif // ALSA_HANDLER_HPP
