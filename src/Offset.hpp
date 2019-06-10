#pragma once

#include "bogaudio.hpp"
#include "disable_output_limit.hpp"

extern Model* modelOffset;

namespace bogaudio {

struct Offset : DisableOutputLimitModule {
	enum ParamIds {
		OFFSET_PARAM,
		SCALE_PARAM,
		NUM_PARAMS
	};

	enum InputIds {
		OFFSET_INPUT,
		SCALE_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	Offset() : DisableOutputLimitModule(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {
		configParam(OFFSET_PARAM, -1.0f, 1.0f, 0.0f, "Offset", " V", 0.0f, 5.0f);
		configParam(SCALE_PARAM, -1.0f, 1.0f, 0.316f, "Scale", "%", 0.0f, 100.0f);
	}

	void process(const ProcessArgs& args) override;

	float knobValue(const Param& knob, Input& cv) const;
};

} // namespace bogaudio
