#pragma once

#include "bogaudio.hpp"
#include "addressable_sequence.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelOneEight;

namespace bogaudio {

struct OneEight : AddressableSequenceModule {
	enum ParamsIds {
		STEPS_PARAM,
		DIRECTION_PARAM,
		SELECT_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CLOCK_INPUT,
		RESET_INPUT,
		SELECT_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		OUT7_OUTPUT,
		OUT8_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		OUT1_LIGHT,
		OUT2_LIGHT,
		OUT3_LIGHT,
		OUT4_LIGHT,
		OUT5_LIGHT,
		OUT6_LIGHT,
		OUT7_LIGHT,
		OUT8_LIGHT,
		NUM_LIGHTS
	};

	float _lightSums[8] {};

	OneEight() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(STEPS_PARAM, 1.0f, 8.0f, 8.0f, "Steps");
		paramQuantities[STEPS_PARAM]->snapEnabled = true;
		configSwitch(DIRECTION_PARAM, 0.0f, 1.0f, 1.0f, "Direction", {"Reverse", "Forward"});
		configParam(SELECT_PARAM, 0.0f, 7.0f, 0.0f, "Select step");
		paramQuantities[SELECT_PARAM]->snapEnabled = true;
		setInputIDs(CLOCK_INPUT, SELECT_INPUT);
		configBypass(IN_INPUT, OUT1_OUTPUT);

		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");
		configInput(SELECT_INPUT, "Select CV");
		configInput(IN_INPUT, "Signal");

		configOutput(OUT1_OUTPUT, "Signal 1");
		configOutput(OUT2_OUTPUT, "Signal 2");
		configOutput(OUT3_OUTPUT, "Signal 3");
		configOutput(OUT4_OUTPUT, "Signal 4");
		configOutput(OUT5_OUTPUT, "Signal 5");
		configOutput(OUT6_OUTPUT, "Signal 6");
		configOutput(OUT7_OUTPUT, "Signal 7");
		configOutput(OUT8_OUTPUT, "Signal 8");
	}

	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void postProcessAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
