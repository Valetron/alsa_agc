#ifndef ALSA_HANDLER_HPP
#define ALSA_HANDLER_HPP

extern "C"
{
#include <alsa/asoundlib.h>
}

class AlsaHandler final
{
public:
    AlsaHandler();
    ~AlsaHandler();
    void run();

private:
    snd_pcm_t* m_capture {nullptr};
    snd_pcm_t* m_playback {nullptr};
};

#endif // ALSA_HANDLER_HPP
