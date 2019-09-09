#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelCVD;

namespace bogaudio {

struct CVD : BGModule {
	enum ParamsIds {
		TIME_PARAM,
		TIME_SCALE_PARAM,
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

	CVD()
	:  _delay(1000.0f, 10000.0f)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(TIME_PARAM, 0.0f, 1.0f, 0.5f, "Time base");
		configParam(TIME_SCALE_PARAM, 0.0f, 2.0f, 1.0f, "Time scale", "", 10.0f, 0.1f);
		configParam(MIX_PARAM, -1.0f, 1.0f, 0.0f, "Dry wet mix", "%", 0.0f, 100.0f);

		sampleRateChange();
	}

	void sampleRateChange() override;
	void processChannel(const ProcessArgs& args, int _c) override;
};

} // namespace bogaudio
