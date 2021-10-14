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

	bool _linear = false;
	float _mix[maxChannels] {};
	float _curveIn[maxChannels];
	bogaudio::dsp::SlewLimiter _mixSL[maxChannels];
	CrossFader _mixer[maxChannels];

	XFade() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(MIX_PARAM, -1.0f, 1.0f, 0.0f, "Mix", "%", 0.0f, 100.0f);
		configParam(CURVE_PARAM, 0.0f, 1.0f, 0.5f, "Curve");
		configSwitch(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "Linear", {"Decibels", "Amplitude"});

		configInput(MIX_INPUT, "Mix CV");
		configInput(A_INPUT, "Signal A");
		configInput(B_INPUT, "Signal B");

		configOutput(OUT_OUTPUT, "Signal");

		sampleRateChange();
		for (int c = 0; c < maxChannels; ++c) {
			_curveIn[c] = -1.0f;
		}
	}

	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
