#ifndef ALSA_HANDLER_HPP
#define ALSA_HANDLER_HPP

#include <memory>
#include <utility>

extern "C"
{
#include <alsa/asoundlib.h>
}

#include "Params.hpp"
// #include "module.hpp"
#include "Agc.hpp"

class AlsaHandler final
{
public:
    AlsaHandler(Params&& params);
    ~AlsaHandler();
    void run();

private:
    std::pair<bool, std::string> setHwParams(snd_pcm_t* handle);
    void close();

private:
    Params m_params;
    snd_pcm_t* m_capture {nullptr};
    snd_pcm_t* m_playback {nullptr};
    bool m_isRunning {false};
    std::unique_ptr<Agc> m_module {nullptr};
};

#endif // ALSA_HANDLER_HPP
