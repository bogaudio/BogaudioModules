#pragma once

#include "bogaudio.hpp"
#include "addressable_sequence.hpp"

extern Model* modelAddrSeq;

namespace bogaudio {

struct AddrSeq : OutputRangeAddressableSequenceModule {
	enum ParamsIds {
		STEPS_PARAM,
		DIRECTION_PARAM,
		SELECT_PARAM,
		OUT1_PARAM,
		OUT2_PARAM,
		OUT3_PARAM,
		OUT4_PARAM,
		OUT5_PARAM,
		OUT6_PARAM,
		OUT7_PARAM,
		OUT8_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
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

	AddrSeq() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(STEPS_PARAM, 1.0f, 8.0f, 8.0f, "Steps");
		configParam(DIRECTION_PARAM, 0.0f, 1.0f, 1.0f, "Direction");
		configParam(SELECT_PARAM, 0.0f, 7.0f, 0.0f, "Select step");
		configParam<OutputRangeParamQuantity>(OUT1_PARAM, -1.0f, 1.0f, 0.0f, "Step 1", " V");
		configParam<OutputRangeParamQuantity>(OUT2_PARAM, -1.0f, 1.0f, 0.0f, "Step 2", " V");
		configParam<OutputRangeParamQuantity>(OUT3_PARAM, -1.0f, 1.0f, 0.0f, "Step 3", " V");
		configParam<OutputRangeParamQuantity>(OUT4_PARAM, -1.0f, 1.0f, 0.0f, "Step 4", " V");
		configParam<OutputRangeParamQuantity>(OUT5_PARAM, -1.0f, 1.0f, 0.0f, "Step 5", " V");
		configParam<OutputRangeParamQuantity>(OUT6_PARAM, -1.0f, 1.0f, 0.0f, "Step 6", " V");
		configParam<OutputRangeParamQuantity>(OUT7_PARAM, -1.0f, 1.0f, 0.0f, "Step 7", " V");
		configParam<OutputRangeParamQuantity>(OUT8_PARAM, -1.0f, 1.0f, 0.0f, "Step 8", " V");
		setInputIDs(CLOCK_INPUT, SELECT_INPUT);
	}

	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void postProcessAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
