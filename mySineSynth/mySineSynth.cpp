

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cmath>

#include "lv2.h"
#include "lv2/atom/atom.h"
#include "lv2/urid/urid.h"
#include "lv2/midi/midi.h"
#include "lv2/core/lv2_util.h"
#include "lv2/atom/util.h"

enum ControlPorts
{
  CONTROL_ATTACK = 0,
  CONTROL_DECAY = 1,
  CONTROL_SUSTAIN = 2,
  CONTROL_RELEASE = 3,
  CONTROL_LEVEL = 4,
  CONTROL_NR = 5
};

enum PortGroups
{
  PORT_MIDI_IN = 0,
  PORT_AUDIO_OUT = 1,
  PORT_CONTROL = 2,
  PORT_NR = 3
};

struct Urids
{
  LV2_URID midi_MidiEvent;
};

class MySineSynth
{
public:
  MySineSynth(const double sampleRate, const LV2_Feature *const *features);

  void connectPort(const uint32_t port, void *data_location);
  void activate();
  void run(const uint32_t sample_count);

private:
  void play(const uint32_t start, const uint32_t end);

  const LV2_Atom_Sequence *midi_in_ptr;
  float *audio_out_ptr;
  const float *control_ptr[CONTROL_NR];
  double rate;
  double position;
  LV2_URID_Map *map;
  Urids urids;
};

MySineSynth::MySineSynth(const double sampleRate, const LV2_Feature *const *features)
    : midi_in_ptr(nullptr),
      audio_out_ptr(nullptr),
      control_ptr{nullptr},
      rate(sampleRate),
      position(0.0),
      map(nullptr)
{
  const char *missing = lv2_features_query(
      features,
      LV2_URID__map, &map, true,
      NULL);

  if (missing)
    throw;

  urids.midi_MidiEvent = map->map(map->handle, LV2_MIDI__MidiEvent);
}

void MySineSynth::connectPort(const uint32_t port, void *data_location)
{
  switch (port)
  {
  case PORT_MIDI_IN:
    midi_in_ptr = (const LV2_Atom_Sequence *)data_location;
    break;
  case PORT_AUDIO_OUT:
    audio_out_ptr = (float *)data_location;
    break;
  default:
    if (port < PORT_CONTROL + CONTROL_NR)
    {
      control_ptr[port - PORT_CONTROL] = (const float *)data_location;
    }
    break;
  }
}

void MySineSynth::activate()
{
}

void MySineSynth::play(const uint32_t start, const uint32_t end)
{
  for (uint32_t i = start; i < end; ++i)
  {
    // TODO
  }
}

void MySineSynth::run(const uint32_t sample_count)
{
  if ((!audio_out_ptr) || (!midi_in_ptr))
    return;

  for (int i = 0; i < CONTROL_NR; ++i)
  {
    if (!control_ptr[i])
      return;
  }

  uint32_t last_frame = 0;
  LV2_ATOM_SEQUENCE_FOREACH(midi_in_ptr, ev)
  {
    // Play until event
    const uint32_t frame = ev->time.frames;
    play(last_frame, frame);
    last_frame = frame;

    if (ev->body.type == urids.midi_MidiEvent)
    {
      const uint8_t *const msg = (const uint8_t *)(ev + 1);
      const uint8_t typ = lv2_midi_message_type(msg);

      switch (typ)
      {
      case LV2_MIDI_MSG_NOTE_ON:
        break;
      case LV2_MIDI_MSG_NOTE_OFF:
        break;
      case LV2_MIDI_MSG_CONTROLLER:
        break;
      default:
        break;
      }
    }
  }

  // play remaining samples
  play(last_frame, sample_count);
}

static LV2_Handle instantiate(const struct LV2_Descriptor *descriptor, double sample_rate, const char *bundle_path, const LV2_Feature *const *features)
{
  MySineSynth *m = new MySineSynth(sample_rate, features);
  return m;
}

static void connect_port(LV2_Handle instance, uint32_t port, void *data_location)
{
  MySineSynth *m = (MySineSynth *)instance;
  if (m)
    m->connectPort(port, data_location);
}

static void activate(LV2_Handle instance)
{
  MySineSynth *m = (MySineSynth *)instance;
  if (m)
    m->activate();
}

static void run(LV2_Handle instance, uint32_t sample_count)
{
  MySineSynth *m = (MySineSynth *)instance;
  if (m)
    m->run(sample_count);
}

static void deactivate(LV2_Handle instance)
{
  /* don't need */
}

static void cleanup(LV2_Handle instance)
{
  MySineSynth *m = (MySineSynth *)instance;
  if (m)
    delete m;
}

static const void *extension_data(const char *uri)
{
  return NULL;
}

static LV2_Descriptor const descriptor =
    {
        "https://github.com/oswald2/lv2tutorial/MySineSynth",
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