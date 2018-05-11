#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelCVDelay;

namespace bogaudio {

struct CVDelay : Module {
	enum ParamsIds {
		TIME_PARAM,
		MIX_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		TIME_INPUT,
		MIX_INPUT,
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

	DelayLine _delay;
	CrossFader _mix;

	CVDelay() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		onSampleRateChange();
	}

	virtual void onSampleRateChange() override;
	virtual void step() override;
};

} // namespace bogaudio
