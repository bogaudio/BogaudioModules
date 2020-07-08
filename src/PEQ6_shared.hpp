#pragma once

#include "bogaudio.hpp"
#include "expanders.hpp"
#include "parametric_eq.hpp"

extern Model* modelPEQ6;
extern Model* modelPEQ6XF;

namespace bogaudio {

struct PEQ6ExpanderMessage : ExpanderMessage {
	float outs[BGModule::maxChannels][6] {};
	float frequencies[BGModule::maxChannels][6] {};
	float bandwidths[BGModule::maxChannels] {};
};

} // namespace bogaudio
