#include <unordered_map>

#include "AlsaHandler.hpp"

namespace
{
const std::unordered_map<std::string, snd_pcm_format_t> g_sampleFormatNameType = {
    {"S16_LE", SND_PCM_FORMAT_S16_LE}
};
}

AlsaHandler::AlsaHandler(Params&& params) : m_params{std::move(params)}
{
    int error {0};

    if ((error = snd_pcm_open(&m_capture, m_params.InputDevice.data(), SND_PCM_STREAM_CAPTURE, 0)) < 0)
        throw std::runtime_error("Failed to open capture device: " + std::string(snd_strerror(error)));

    if ((error = snd_pcm_open(&m_playback, m_params.OutputDevice.data(), SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
        snd_pcm_close(m_capture);
        throw std::runtime_error("Failed to open playback device: " + std::string(snd_strerror(error)));
    }

    auto res = setHwParams(m_capture);
    if (!res.first)
    {
        close();
        throw std::runtime_error(res.second);
    }

    res = setHwParams(m_playback);
    if (!res.first)
    {
        close();
        throw std::runtime_error(res.second);
    }
}

AlsaHandler::~AlsaHandler()
{
    // TODO: close pcm handlers
    close();
}

void AlsaHandler::run()
{

}

std::pair<bool, std::string> AlsaHandler::setHwParams(snd_pcm_t* handle)
{
    snd_pcm_hw_params_t* params {nullptr};
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);

    if (snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
        return std::make_pair(false, "Failed to set access mod");

    if (snd_pcm_hw_params_set_format(handle, params, g_sampleFormatNameType.at(m_params.SampleFormat)) < 0)
        return std::make_pair(false, "Failed to set sample format");

    if (snd_pcm_hw_params_set_channels(handle, params, m_params.Channels) < 0)
        return std::make_pair(false, "Failed to set channels");

    if (snd_pcm_hw_params_set_rate(handle, params, m_params.SampleRate, 0) < 0)
        return std::make_pair(false, "Failed to set sample rate");

    if (snd_pcm_hw_params(handle, params) < 0)
        return std::make_pair(false, "Failed to set access mod");

    return std::make_pair(true, "Ok");
}

void AlsaHandler::close()
{
    if (m_capture)
        snd_pcm_close(m_capture);

    if (m_playback)
        snd_pcm_close(m_playback);
}
