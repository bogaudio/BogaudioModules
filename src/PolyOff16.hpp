#pragma once

#include "bogaudio.hpp"
#include "output_range.hpp"

extern Model* modelPolyOff16;

namespace bogaudio {

struct PolyOff16 : OutputRangeModule<BGModule> {
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
		OFFSET9_PARAM,
		SCALE9_PARAM,
		OFFSET10_PARAM,
		SCALE10_PARAM,
		OFFSET11_PARAM,
		SCALE11_PARAM,
		OFFSET12_PARAM,
		SCALE12_PARAM,
		OFFSET13_PARAM,
		SCALE13_PARAM,
		OFFSET14_PARAM,
		SCALE14_PARAM,
		OFFSET15_PARAM,
		SCALE15_PARAM,
		OFFSET16_PARAM,
		SCALE16_PARAM,
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
		CV9_INPUT,
		CV10_INPUT,
		CV11_INPUT,
		CV12_INPUT,
		CV13_INPUT,
		CV14_INPUT,
		CV15_INPUT,
		CV16_INPUT,
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

	PolyOff16() {
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
		configParam<OutputRangeParamQuantity>(OFFSET9_PARAM, -1.0f, 1.0f, 0.0f, "Channel 9 offset");
		configParam(SCALE9_PARAM, -1.0f, 1.0f, 1.0f, "Channel 9 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET10_PARAM, -1.0f, 1.0f, 0.0f, "Channel 10 offset");
		configParam(SCALE10_PARAM, -1.0f, 1.0f, 1.0f, "Channel 10 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET11_PARAM, -1.0f, 1.0f, 0.0f, "Channel 11 offset");
		configParam(SCALE11_PARAM, -1.0f, 1.0f, 1.0f, "Channel 11 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET12_PARAM, -1.0f, 1.0f, 0.0f, "Channel 12 offset");
		configParam(SCALE12_PARAM, -1.0f, 1.0f, 1.0f, "Channel 12 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET13_PARAM, -1.0f, 1.0f, 0.0f, "Channel 13 offset");
		configParam(SCALE13_PARAM, -1.0f, 1.0f, 1.0f, "Channel 13 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET14_PARAM, -1.0f, 1.0f, 0.0f, "Channel 14 offset");
		configParam(SCALE14_PARAM, -1.0f, 1.0f, 1.0f, "Channel 14 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET15_PARAM, -1.0f, 1.0f, 0.0f, "Channel 15 offset");
		configParam(SCALE15_PARAM, -1.0f, 1.0f, 1.0f, "Channel 15 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET16_PARAM, -1.0f, 1.0f, 0.0f, "Channel 16 offset");
		configParam(SCALE16_PARAM, -1.0f, 1.0f, 1.0f, "Channel 16 scale", "%", 0.0f, 100.0f);
		configParam(CHANNELS_PARAM, 1.0f, 16.0f, 1.0f, "Polyphony channels");
	}

	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
