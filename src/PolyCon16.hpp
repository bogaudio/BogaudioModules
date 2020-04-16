#pragma once

#include "bogaudio.hpp"
#include "output_range.hpp"

extern Model* modelPolyCon16;

namespace bogaudio {

struct PolyCon16 : OutputRangeModule<BGModule> {
	enum ParamsIds {
		CHANNELS_PARAM,
		CHANNEL1_PARAM,
		CHANNEL2_PARAM,
		CHANNEL3_PARAM,
		CHANNEL4_PARAM,
		CHANNEL5_PARAM,
		CHANNEL6_PARAM,
		CHANNEL7_PARAM,
		CHANNEL8_PARAM,
		CHANNEL9_PARAM,
		CHANNEL10_PARAM,
		CHANNEL11_PARAM,
		CHANNEL12_PARAM,
		CHANNEL13_PARAM,
		CHANNEL14_PARAM,
		CHANNEL15_PARAM,
		CHANNEL16_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CHANNELS_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		CHANNEL1_LIGHT,
		CHANNEL2_LIGHT,
		CHANNEL3_LIGHT,
		CHANNEL4_LIGHT,
		CHANNEL5_LIGHT,
		CHANNEL6_LIGHT,
		CHANNEL7_LIGHT,
		CHANNEL8_LIGHT,
		CHANNEL9_LIGHT,
		CHANNEL10_LIGHT,
		CHANNEL11_LIGHT,
		CHANNEL12_LIGHT,
		CHANNEL13_LIGHT,
		CHANNEL14_LIGHT,
		CHANNEL15_LIGHT,
		CHANNEL16_LIGHT,
		NUM_LIGHTS
	};

	PolyCon16() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CHANNELS_PARAM, 1.0f, 16.0f, 1.0f, "Polyphony channels");
		configParam<OutputRangeParamQuantity>(CHANNEL1_PARAM, -1.0f, 1.0f, 0.0f, "Channel 1", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL9_PARAM, -1.0f, 1.0f, 0.0f, "Channel 9", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL10_PARAM, -1.0f, 1.0f, 0.0f, "Channel 10", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL11_PARAM, -1.0f, 1.0f, 0.0f, "Channel 11", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL12_PARAM, -1.0f, 1.0f, 0.0f, "Channel 12", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL13_PARAM, -1.0f, 1.0f, 0.0f, "Channel 13", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL14_PARAM, -1.0f, 1.0f, 0.0f, "Channel 14", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL15_PARAM, -1.0f, 1.0f, 0.0f, "Channel 15", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL16_PARAM, -1.0f, 1.0f, 0.0f, "Channel 16", " V");
	}

	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
