#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelVCA;

namespace bogaudio {

struct VCA : Module {
	enum ParamsIds {
		LEVEL1_PARAM,
		LEVEL2_PARAM,
		LINEAR_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV1_INPUT,
		IN1_INPUT,
		CV2_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		LINEAR_LIGHT,
		NUM_LIGHTS
	};

	Amplifier _amplifier1;
	bogaudio::dsp::SlewLimiter _levelSL1;
	Amplifier _amplifier2;
	bogaudio::dsp::SlewLimiter _levelSL2;

	VCA() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(LEVEL1_PARAM, 0.0f, 1.0f, 0.8f, "level1");
		configParam(LEVEL2_PARAM, 0.0f, 1.0f, 0.8f, "level2");
		configParam(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "linear");

		onSampleRateChange();
	}

	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
	void channelStep(Input& input, Output& output, Param& knob, Input& cv, Amplifier& amplifier, bogaudio::dsp::SlewLimiter& levelSL, bool linear);
};

} // namespace bogaudio
