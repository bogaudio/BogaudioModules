#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelUMix;

namespace bogaudio {

struct UMix : BGModule {
	enum ParamsIds {
		LEVEL_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		IN5_INPUT,
		IN6_INPUT,
		IN7_INPUT,
		IN8_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	bool _sum = true;
	bool _cvMode = false;
	Saturator _saturator[maxChannels];

	UMix() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(LEVEL_PARAM, 0.0f, 1.0f, 1.0f, "Level", "%", 0.0f, 100.0f);
	}

	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	bool active() override;
	int channels() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
