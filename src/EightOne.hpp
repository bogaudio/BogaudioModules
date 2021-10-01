#pragma once

#include "bogaudio.hpp"
#include "addressable_sequence.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelEightOne;

namespace bogaudio {

struct EightOne : AddressableSequenceModule {
	enum ParamsIds {
		STEPS_PARAM,
		DIRECTION_PARAM,
		SELECT_PARAM,
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
		CLOCK_INPUT,
		RESET_INPUT,
		SELECT_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		IN1_LIGHT,
		IN2_LIGHT,
		IN3_LIGHT,
		IN4_LIGHT,
		IN5_LIGHT,
		IN6_LIGHT,
		IN7_LIGHT,
		IN8_LIGHT,
		NUM_LIGHTS
	};

	float _lightSums[8] {};

	EightOne() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(STEPS_PARAM, 1.0f, 8.0f, 8.0f, "Steps");
		configParam(DIRECTION_PARAM, 0.0f, 1.0f, 1.0f, "Direction");
		configParam(SELECT_PARAM, 0.0f, 7.0f, 0.0f, "Select step");
		setInputIDs(CLOCK_INPUT, SELECT_INPUT);
		configBypass(IN1_INPUT, OUT_OUTPUT);
	}

	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void postProcessAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
