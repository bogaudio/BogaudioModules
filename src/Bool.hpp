#pragma once

#include "bogaudio.hpp"

extern Model* modelBool;

namespace bogaudio {

struct Bool : BGModule {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		A_INPUT,
		B_INPUT,
		NOT_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		AND_OUTPUT,
		OR_OUTPUT,
		XOR_OUTPUT,
		NOT_OUTPUT,
		NUM_OUTPUTS
	};

	Bool() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

		configInput(A_INPUT, "Logic signal A");
		configInput(B_INPUT, "Logic signal B");
		configInput(NOT_INPUT, "NOT signal");

		configOutput(AND_OUTPUT, "AND");
		configOutput(OR_OUTPUT, "OR");
		configOutput(XOR_OUTPUT, "XOR");
		configOutput(NOT_OUTPUT, "NOT");
	}

	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
