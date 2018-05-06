#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelVCAmp;

namespace bogaudio {

struct VCAmp : Module {
	enum ParamsIds {
		LEVEL_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	const float maxDecibels = 12.0f;
	const float minDecibels = Amplifier::minDecibels;
	Amplifier _amplifier;

	VCAmp() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
	}

	virtual void step() override;
};

} // namespace bogaudio
