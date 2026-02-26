#define PIC 1 // NOTE: https://github.com/alsa-project/alsa-lib/issues/289

#include <memory>
#include <cstring>
#include <string_view>

extern "C"
{
#include <alsa/asoundlib.h>
#include <alsa/pcm_external.h>
#include <alsa/conf.h>
}

#include "AGC.hpp"
#include "params.hpp"

using namespace valetron::agc;
using namespace valetron::agc::alsa_plugin;

namespace
{
const auto g_errorGetParamTemplate {"Invalid value for %s parameter"};

bool getIntParam(snd_config_t* cfg, const  char* id, const std::string_view paramName, uint32_t& out)
{
  long res {0};
  int err {0};

  if (std::strcmp(id, paramName.data()))
    return true;

  err = snd_config_get_integer(cfg, &res);
  if (err < 0)
  {
    SNDERR(g_errorGetParamTemplate, id);
    return false;
  }

  out = res;

  return true;
}

bool getDoubleParam(snd_config_t* cfg, const  char* id, const std::string_view paramName, double& out)
{
  double res {0};
  int err {0};

  if (std::strcmp(id, paramName.data()))
    return true;

  err = snd_config_get_real(cfg, &res);
  if (err < 0)
  {
    SNDERR(g_errorGetParamTemplate, id);
    return false;
  }

  out = res;

  return true;
}

using TAgc = lib::AutomaticGainControl;
std::unique_ptr<TAgc> g_agc {nullptr};
}

static snd_pcm_sframes_t callback_transfer(snd_pcm_extplug_t* ext, const snd_pcm_channel_area_t* dst_areas,
                                            snd_pcm_uframes_t dst_offset, const snd_pcm_channel_area_t* src_areas,
                                            snd_pcm_uframes_t src_offset, snd_pcm_uframes_t size)
{
  // TODO: same as utils/main.cpp
  return size;
}

static int callback_init(snd_pcm_extplug_t* ext)
{
  const auto params = reinterpret_cast<const PluginParams*>(ext->private_data);
  g_agc = std::make_unique<TAgc>(params->FrameLenMs, params->Channels, params->Rate, params->FilterSize);
  return 0;
}

static int callback_close(snd_pcm_extplug_t* ext)
{
  return 0;
}

static const snd_pcm_extplug_callback_t plugin_agc_callback = {
  .transfer = callback_transfer,
  .close = callback_close,
  .init = callback_init,
};

extern "C"
{
SND_PCM_PLUGIN_DEFINE_FUNC(alsa_agc)
{
  PluginParams* params {nullptr};

  snd_pcm_extplug_t ext {};
  snd_config_iterator_t i {nullptr};
  snd_config_iterator_t next {nullptr};
  snd_config_t* slave {nullptr};

  int err {0};

  snd_config_for_each(i, next, conf)
  {
    auto confEntry = snd_config_iterator_entry(i);
    const char *id {nullptr};
    if (snd_config_get_id(confEntry, &id) < 0)
      continue;

    if (0 == std::strcmp(id, "comment") || 0 == std::strcmp(id, "type"))
      continue;

    if (0 == std::strcmp(id, "slave"))
    {
      slave = confEntry;
      continue;
    }

    if (getIntParam(confEntry, id, "frame_len", params->FrameLenMs))
      return -EINVAL;

    // if (getIntParam(confEntry, id, "channels", params->Channels))
    //   return -EINVAL;

    if (getIntParam(confEntry, id, "rate", params->Rate))
      return -EINVAL;

    if (getIntParam(confEntry, id, "filter_size", params->FilterSize))
      return -EINVAL;

    if (getDoubleParam(confEntry, id, "peak", params->Peak))
      return -EINVAL;

    if (getDoubleParam(confEntry, id, "gain", params->Gain))
      return -EINVAL;

    if (getDoubleParam(confEntry, id, "rms", params->Rms))
      return -EINVAL;

    SNDERR("Unknown field %s", id);
    return -EINVAL;
  }

  if (!slave)
  {
    SNDERR("No slave defined");
    return -EINVAL;
  }

  ext.version = SND_PCM_EXTPLUG_VERSION;
  ext.name = "AGC filter plugin by Valetron";
  ext.callback = &plugin_agc_callback;
  ext.private_data = params;

  err = snd_pcm_extplug_create(&ext, name, root, slave, stream, mode);

  if (err < 0)
  {
    SNDERR("Error");
    return err;
  }

  snd_pcm_extplug_set_param(&ext, SND_PCM_EXTPLUG_HW_CHANNELS, lib::constants::DefaultChannel);
  snd_pcm_extplug_set_slave_param(&ext, SND_PCM_EXTPLUG_HW_CHANNELS, lib::constants::DefaultChannel);
  snd_pcm_extplug_set_param(&ext, SND_PCM_EXTPLUG_HW_FORMAT, SND_PCM_FORMAT_S16);
  snd_pcm_extplug_set_slave_param(&ext, SND_PCM_EXTPLUG_HW_FORMAT, SND_PCM_FORMAT_S16);

  *pcmp = ext.pcm;
  return 0;
}

SND_PCM_PLUGIN_SYMBOL(alsa_agc);

} // extern "C"
