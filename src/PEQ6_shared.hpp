#pragma once

#include "bogaudio.hpp"
#include "expanders.hpp"
#include "parametric_eq.hpp"

extern Model* modelPEQ6;
extern Model* modelPEQ6XO;

namespace bogaudio {

struct PEQ6ExpanderMessage : ExpanderMessage {
	float outs[BGModule::maxChannels][6];

	PEQ6ExpanderMessage() {
		for (int c = 0; c < BGModule::maxChannels; ++c) {
			std::fill(outs[c], outs[c] + 6, 0.0f);
		}
	}
};

} // namespace bogaudio
