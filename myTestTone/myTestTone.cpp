

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cmath>

#include "lv2.h"

class MyTestTone
{
public:
  MyTestTone(const double sampleRate);

  void connectPort(const uint32_t port, void *data_location);
  void activate();
  void run(const uint32_t sample_count);

private:
  float *audio_out_ptr;
  float *freq_ptr;
  float *level_ptr;
  double rate;
  double position;
};

MyTestTone::MyTestTone(const double sampleRate) : audio_out_ptr(nullptr),
                                                  freq_ptr(nullptr),
                                                  level_ptr(nullptr),
                                                  rate(sampleRate),
                                                  position(0.0)
{
}

void MyTestTone::connectPort(const uint32_t port, void *data_location)
{
  switch (port)
  {
  case 0:
    audio_out_ptr = (float *)data_location;
    break;
  case 1:
    freq_ptr = (float *)data_location;
    break;
  case 2:
    level_ptr = (float *)data_location;
    break;
  default:
    break;
  }
}

void MyTestTone::activate()
{
  position = 0.0;
}


void MyTestTone::run(const uint32_t sample_count)
{
  if((!audio_out_ptr) || (!freq_ptr) || (!level_ptr)) return;

  for(uint32_t i=0; i<sample_count; ++i)
  {
    audio_out_ptr[i] = sin(2.0 * M_PI * position) * *level_ptr;
    position += *freq_ptr / rate;
  }
}




static LV2_Handle instantiate(const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
  MyTestTone* m = new MyTestTone(sample_rate);
  return m;
}

static void connect_port(LV2_Handle instance, uint32_t port, void *data_location)
{
  MyTestTone *m = (MyTestTone *)instance;
  if(m) m->connectPort(port, data_location);
}

static void activate(LV2_Handle instance)
{
  MyTestTone *m = (MyTestTone *)instance;
  if(m) m->activate();
}

static void run(LV2_Handle instance, uint32_t sample_count)
{
  MyTestTone *m = (MyTestTone *)instance;
  if(m) m->run(sample_count);
}

static void deactivate(LV2_Handle instance)
{
  /* don't need */
}

static void cleanup(LV2_Handle instance)
{
  MyTestTone *m = (MyTestTone *)instance;
  if(m) delete m;
}

static const void *extension_data(const char *uri)
{
  return NULL;
}

static LV2_Descriptor const descriptor =
    {
        "https://github.com/oswald2/lv2tutorial/myTestTone",
        instantiate,
        connect_port,
        activate,
        run,
        deactivate,
        cleanup,
        extension_data};

extern "C" LV2_SYMBOL_EXPORT const LV2_Descriptor *lv2_descriptor(uint32_t index)
{
  if (index == 0)
    return &descriptor;
  else
    return NULL;
}