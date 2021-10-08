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

	bool _offsetFirst = false;

	PolyOff8() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<OutputRangeParamQuantity>(OFFSET1_PARAM, -1.0f, 1.0f, 0.0f, "Channel 1 offset", " V");
		configParam(SCALE1_PARAM, -1.0f, 1.0f, 1.0f, "Channel 1 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET2_PARAM, -1.0f, 1.0f, 0.0f, "Channel 2 offset", " V");
		configParam(SCALE2_PARAM, -1.0f, 1.0f, 1.0f, "Channel 2 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET3_PARAM, -1.0f, 1.0f, 0.0f, "Channel 3 offset", " V");
		configParam(SCALE3_PARAM, -1.0f, 1.0f, 1.0f, "Channel 3 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET4_PARAM, -1.0f, 1.0f, 0.0f, "Channel 4 offset", " V");
		configParam(SCALE4_PARAM, -1.0f, 1.0f, 1.0f, "Channel 4 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET5_PARAM, -1.0f, 1.0f, 0.0f, "Channel 5 offset", " V");
		configParam(SCALE5_PARAM, -1.0f, 1.0f, 1.0f, "Channel 5 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET6_PARAM, -1.0f, 1.0f, 0.0f, "Channel 6 offset", " V");
		configParam(SCALE6_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET7_PARAM, -1.0f, 1.0f, 0.0f, "Channel 7 offset", " V");
		configParam(SCALE7_PARAM, -1.0f, 1.0f, 1.0f, "Channel 7 scale", "%", 0.0f, 100.0f);
		configParam<OutputRangeParamQuantity>(OFFSET8_PARAM, -1.0f, 1.0f, 0.0f, "Channel 8 offset", " V");
		configParam(SCALE8_PARAM, -1.0f, 1.0f, 1.0f, "Channel 8 scale", "%", 0.0f, 100.0f);
		configParam(CHANNELS_PARAM, 1.0f, 8.0f, 1.0f, "Polyphony channels");
		paramQuantities[CHANNELS_PARAM]->snapEnabled = true;

		configInput(CV1_INPUT, "CV1");
		configInput(CV2_INPUT, "CV2");
		configInput(CV3_INPUT, "CV3");
		configInput(CV4_INPUT, "CV4");
		configInput(CV5_INPUT, "CV5");
		configInput(CV6_INPUT, "CV6");
		configInput(CV7_INPUT, "CV7");
		configInput(CV8_INPUT, "CV8");
		configInput(IN_INPUT, "IN");

		configOutput(OUT_OUTPUT, "OUT");
	}

	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
