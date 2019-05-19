#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelFollow;

namespace bogaudio {

struct Follow : Module {
	enum ParamsIds {
		RESPONSE_PARAM,
		SCALE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		RESPONSE_INPUT,
		SCALE_INPUT,
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

	RootMeanSquare _rms;

	Follow()
	:  _rms(1000.0f, 1.0f, 500.0f)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RESPONSE_PARAM, 0.0f, 1.0f, 0.3f, "response");
		configParam(SCALE_PARAM, 0.0f, 1.0f, 1.0f, "scale");

		onSampleRateChange();
	}

	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
