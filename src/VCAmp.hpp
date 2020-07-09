#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/utility.hpp"
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

	const float maxDecibels = 12.0f;
	const float minDecibels = Amplifier::minDecibels;
	Amplifier _amplifier[maxChannels];
	bogaudio::dsp::SlewLimiter _levelSL[maxChannels];
	Saturator _saturator[maxChannels];
	RootMeanSquare _rms[maxChannels];
	float _rmsLevel = 0.0f;

	VCAmp() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(LEVEL_PARAM, 0.0f, 1.0f, fabs(minDecibels) / (maxDecibels - minDecibels), "Level", " dB", 0.0f, maxDecibels - minDecibels, minDecibels);

		sampleRateChange();
		for (int c = 0; c < maxChannels; ++c) {
			_rms[c].setSensitivity(0.05f);
		}
	}

	void sampleRateChange() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
