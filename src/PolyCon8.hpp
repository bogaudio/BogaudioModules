#pragma once

#include "bogaudio.hpp"
#include "output_range.hpp"
#include "poly_channels.hpp"

extern Model* modelPolyCon8;

namespace bogaudio {

struct PolyCon8 : OutputRangeModule<PolyChannelsModule> {
	enum ParamsIds {
		CHANNEL1_PARAM,
		CHANNEL2_PARAM,
		CHANNEL3_PARAM,
		CHANNEL4_PARAM,
		CHANNEL5_PARAM,
		CHANNEL6_PARAM,
		CHANNEL7_PARAM,
		CHANNEL8_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
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
		NUM_LIGHTS
	};

	PolyCon8() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<OutputRangeParamQuantity>(CHANNEL1_PARAM, -1.0f, 1.0f, 0.0f, "Channel 1", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7", " V");
		configParam<OutputRangeParamQuantity>(CHANNEL8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8", " V");

		configOutput(OUT_OUTPUT, "Signal");
	}

	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
