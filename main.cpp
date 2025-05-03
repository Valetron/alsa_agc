#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <signal.h>
#include <stdbool.h>

#include <iostream>

#include <boost/program_options.hpp>

#include "Params.hpp"

// #define SAMPLE_RATE     8000
// #define CHANNELS        1
// #define FORMAT          SND_PCM_FORMAT_S16_LE
// #define FRAMES_PER_BUF  128

// volatile sig_atomic_t keep_running = 1;

namespace
{
const uint32_t g_sampleRate {8000};
const uint32_t g_channels {1};
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

        if (!vm.count("i") && !vm.count("input"))
        {
            throw std::runtime_error("Input device must be specified");
        }

        if (!vm.count("o") && !vm.count("output"))
        {
            throw std::runtime_error("Output device must be specified");
        }

        if (params.Verbose)
            std::cout << params << "\n";
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error occured: " << ex.what() << "\n";
        return 1;
    }


    return 0;
}

// void int_handler(int dummy) {
//     keep_running = 0;
// }

// int set_hw_params(snd_pcm_t *handle) {
//     snd_pcm_hw_params_t *params;
//     snd_pcm_hw_params_alloca(&params);
//     snd_pcm_hw_params_any(handle, params);

//     if (snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0)
//         return -1;
//     if (snd_pcm_hw_params_set_format(handle, params, FORMAT) < 0)
//         return -1;
//     if (snd_pcm_hw_params_set_channels(handle, params, CHANNELS) < 0)
//         return -1;
//     if (snd_pcm_hw_params_set_rate(handle, params, SAMPLE_RATE, 0) < 0)
//         return -1;
//     // if (snd_pcm_hw_params_set_period_size(handle, params, FRAMES_PER_BUF, 0) < 0)
//     //     return -1;

//     if (snd_pcm_hw_params(handle, params) < 0)
//         return -1;

//     return 0;
// }

// int main() {
//     snd_pcm_t *capture_handle;
//     snd_pcm_t *playback_handle;
//     int err;

//     signal(SIGINT, int_handler);

//     // Открываем устройство захвата
//     if ((err = snd_pcm_open(&capture_handle, "plughw:CARD=Generic_1,DEV=0", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
//         fprintf(stderr, "Не удалось открыть устройство захвата: %s\n", snd_strerror(err));
//         return 1;
//     }

//     // Открываем устройство воспроизведения
//     if ((err = snd_pcm_open(&playback_handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
//         fprintf(stderr, "Не удалось открыть устройство воспроизведения: %s\n", snd_strerror(err));
//         snd_pcm_close(capture_handle);
//         return 1;
//     }

//     // Настройка параметров
//     if (set_hw_params(capture_handle) < 0 || set_hw_params(playback_handle) < 0) {
//         fprintf(stderr, "Не удалось установить параметры устройства\n");
//         snd_pcm_close(capture_handle);
//         snd_pcm_close(playback_handle);
//         return 1;
//     }

//     // Буфер: FRAMES_PER_BUF * 2 байта (16 бит) * 1 канал
//     int16_t buffer[FRAMES_PER_BUF];

//     printf("Передача звука началась (Ctrl+C для выхода)...\n");

//     while (keep_running) {
//         snd_pcm_sframes_t frames;

//         // Чтение звука
//         frames = snd_pcm_readi(capture_handle, buffer, FRAMES_PER_BUF);
//         if (frames < 0) {
//             snd_pcm_prepare(capture_handle);
//             continue;
//         }

//         // Воспроизведение
//         frames = snd_pcm_writei(playback_handle, buffer, FRAMES_PER_BUF);
//         if (frames < 0) {
//             snd_pcm_prepare(playback_handle);
//         }
//     }

//     printf("Завершение...\n");
//     snd_pcm_close(capture_handle);
//     snd_pcm_close(playback_handle);

//     return 0;
// }
