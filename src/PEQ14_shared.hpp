#pragma once

#include "bogaudio.hpp"
#include "expanders.hpp"
#include "parametric_eq.hpp"

extern Model* modelPEQ14;
extern Model* modelPEQ14XO;

namespace bogaudio {

struct PEQ14ExpanderMessage : ExpanderMessage {
	float outs[BGModule::maxChannels][14];

	PEQ14ExpanderMessage() {
		for (int c = 0; c < BGModule::maxChannels; ++c) {
			std::fill(outs[c], outs[c] + 14, 0.0f);
		}
	}
};

} // namespace bogaudio
