#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelFollow;

namespace bogaudio {

struct Follow : BGModule {
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

	RootMeanSquare* _rms[maxChannels] {};

	Follow() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RESPONSE_PARAM, 0.0f, 1.0f, 0.3f, "Sensitivity", "%", 0.0f, 100.0f);
		configParam(SCALE_PARAM, 0.0f, 1.0f, 1.0f, "Scale", "%", 0.0f, 100.0f);

		sampleRateChange();
	}

	bool active() override;
	int channels() override;
	void addEngine(int c) override;
	void removeEngine(int c) override;
	void sampleRateChange() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
