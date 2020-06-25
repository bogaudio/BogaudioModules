#pragma once

#include "bogaudio.hpp"
#include "mixer_expander.hpp"
#include "dsp/signal.hpp"

extern Model* modelMix4;
extern Model* modelMix4x;

namespace bogaudio {

typedef MixerExpanderMessage<4> Mix4ExpanderMessage;

} // namespace bogaudio
