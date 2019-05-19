#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelXFade;

namespace bogaudio {

struct XFade : Module {
	enum ParamsIds {
		MIX_PARAM,
		CURVE_PARAM,
		LINEAR_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		MIX_INPUT,
		A_INPUT,
		B_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		LINEAR_LIGHT,
		NUM_LIGHTS
	};

	bool _linear = false;
	float _mix = 0.0f;
	float _curveIn = -1.0f;
	bogaudio::dsp::SlewLimiter _mixSL;
	CrossFader _mixer;

	XFade() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(MIX_PARAM, -1.0f, 1.0f, 0.0f, "mix");
		configParam(CURVE_PARAM, 0.0f, 1.0f, 0.5f, "curve");
		configParam(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "linear");

		onSampleRateChange();
	}

	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
