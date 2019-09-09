#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelXFade;

namespace bogaudio {

struct XFade : BGModule {
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
		configParam(MIX_PARAM, -1.0f, 1.0f, 0.0f, "Mix", "%", 0.0f, 100.0f);
		configParam(CURVE_PARAM, 0.0f, 1.0f, 0.5f, "Curve");
		configParam(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "Linear");

		sampleRateChange();
	}

	void sampleRateChange() override;
	void processChannel(const ProcessArgs& args, int _c) override;
};

} // namespace bogaudio
