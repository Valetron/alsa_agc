#include <vector>
#include <unordered_map>

#include "AlsaHandler.hpp"
#include "Agc.hpp"

namespace
{
const auto g_readSize {256};
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

    m_module = std::make_unique<Agc>(0.3f, 0.01f, 0.95f, 10.0f, 0.1f);
}

AlsaHandler::~AlsaHandler()
{
    m_isRunning = false;
    close();
}

void AlsaHandler::run()
{
    m_isRunning = true;

    const auto format = snd_pcm_format_value(m_params.SampleFormat.data());
    const auto bufferSize = (m_params.LatencyMax * 2 * snd_pcm_format_physical_width(format) / 8) * m_params.Channels;
    std::vector<uint8_t> buffer(bufferSize, 0);
    while (m_isRunning)
    {
        snd_pcm_sframes_t frames = snd_pcm_readi(m_capture, buffer.data(), g_readSize);
        if (frames < 0)
        {
            snd_pcm_prepare(m_capture);
            continue;
        }

        // buffer.resize(frames);
        m_module->processFrame(buffer);

        frames = snd_pcm_writei(m_playback, buffer.data(), frames);
        if (frames < 0)
            snd_pcm_prepare(m_playback);
    }
}

std::pair<bool, std::string> AlsaHandler::setHwParams(snd_pcm_t* handle)
{
    snd_pcm_hw_params_t* params {nullptr};
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);

    if (snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
        return std::make_pair(false, "Failed to set access mod");

    if (snd_pcm_hw_params_set_format(handle, params, snd_pcm_format_value(m_params.SampleFormat.data())) < 0)
        return std::make_pair(false, "Failed to set sample format");

    if (snd_pcm_hw_params_set_channels(handle, params, m_params.Channels) < 0)
        return std::make_pair(false, "Failed to set channels");

    if (snd_pcm_hw_params_set_rate(handle, params, m_params.SampleRate, 0) < 0)
        return std::make_pair(false, "Failed to set sample rate");

    // TODO: другие параметры?

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
