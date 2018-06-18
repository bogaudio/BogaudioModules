#pragma once

#include "bogaudio.hpp"

extern Model* modelOffset;

namespace bogaudio {

struct Offset : Module {
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

	bool _disableOutputLimit = false;

	Offset() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}

	json_t* toJson() override;
	void fromJson(json_t* root) override;
	void step() override;

	float knobValue(const Param& knob, const Input& cv) const;
};

} // namespace bogaudio
