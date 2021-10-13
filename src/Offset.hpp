#pragma once

#include "bogaudio.hpp"
#include "disable_output_limit.hpp"

extern Model* modelOffset;

namespace bogaudio {

struct Offset : DisableOutputLimitModule {
	enum ParamsIds {
		OFFSET_PARAM,
		SCALE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		OFFSET_INPUT,
		SCALE_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	bool _offsetFirst = false;

	Offset() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(OFFSET_PARAM, -1.0f, 1.0f, 0.0f, "Offset", " V", 0.0f, 10.0f);
		configParam<TenXSquaringParamQuantity>(SCALE_PARAM, -1.0f, 1.0f, 0.31623f, "Scale", "x");
		configBypass(IN_INPUT, OUT_OUTPUT);

		configInput(OFFSET_INPUT, "Offset CV");
		configInput(SCALE_INPUT, "Scale CV");
		configInput(IN_INPUT, "Signal");

		configOutput(OUT_OUTPUT, "OUT");
	}

	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	int channels() override;
	void processChannel(const ProcessArgs& args, int c) override;
	float knobValue(Param& knob, Input& cv, int c) const;
};

} // namespace bogaudio
