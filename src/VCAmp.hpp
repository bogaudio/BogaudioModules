#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelVCAmp;

namespace bogaudio {

struct VCAmp : BGModule {
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
	bogaudio::dsp::SlewLimiter _levelSL;
	Saturator _saturator;
	RootMeanSquare _rms;
	float _rmsLevel = 0.0f;

	VCAmp() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(LEVEL_PARAM, 0.0f, 1.0f, fabs(minDecibels) / (maxDecibels - minDecibels), "Level", " dB", 0.0f, maxDecibels - minDecibels, minDecibels);

		onSampleRateChange();
		_rms.setSensitivity(0.05f);
	}

	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
