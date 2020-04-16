#pragma once

#include "bogaudio.hpp"
#include "output_range.hpp"

extern Model* modelPolyOff8;

namespace bogaudio {

struct PolyOff8 : OutputRangeModule<BGModule> {
	enum ParamsIds {
		OFFSET1_PARAM,
		SCALE1_PARAM,
		OFFSET2_PARAM,
		SCALE2_PARAM,
		OFFSET3_PARAM,
		SCALE3_PARAM,
		OFFSET4_PARAM,
		SCALE4_PARAM,
		OFFSET5_PARAM,
		SCALE5_PARAM,
		OFFSET6_PARAM,
		SCALE6_PARAM,
		OFFSET7_PARAM,
		SCALE7_PARAM,
		OFFSET8_PARAM,
		SCALE8_PARAM,
		CHANNELS_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV1_INPUT,
		CV2_INPUT,
		CV3_INPUT,
		CV4_INPUT,
		CV5_INPUT,
		CV6_INPUT,
		CV7_INPUT,
		CV8_INPUT,
		IN_INPUT,
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

	PolyOff8() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<OutputRangeParamQuantity>(OFFSET1_PARAM, -1.0f, 1.0f, 0.0f, "Channel 1 offset");
		configParam(SCALE1_PARAM, -1.0f, 1.0f, 1.0f, "Channel 1 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2 offset");
		configParam(SCALE2_PARAM, -1.0f, 1.0f, 1.0f, "Channel 2 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3 offset");
		configParam(SCALE3_PARAM, -1.0f, 1.0f, 1.0f, "Channel 3 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4 offset");
		configParam(SCALE4_PARAM, -1.0f, 1.0f, 1.0f, "Channel 4 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5 offset");
		configParam(SCALE5_PARAM, -1.0f, 1.0f, 1.0f, "Channel 5 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6 offset");
		configParam(SCALE6_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7 offset");
		configParam(SCALE7_PARAM, -1.0f, 1.0f, 1.0f, "Channel 7 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8 offset");
		configParam(SCALE8_PARAM, -1.0f, 1.0f, 1.0f, "Channel 8 scale", "%", 0.0f, 100.0f);
		configParam(CHANNELS_PARAM, 1.0f, 8.0f, 1.0f, "Polyphony channels");
	}

	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
