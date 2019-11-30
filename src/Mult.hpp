#pragma once

#include "bogaudio.hpp"

extern Model* modelMult;

namespace bogaudio {

struct Mult : BGModule {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		INA_INPUT,
		INB_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUTA1_OUTPUT,
		OUTA2_OUTPUT,
		OUTA3_OUTPUT,
		OUTB1_OUTPUT,
		OUTB2_OUTPUT,
		OUTB3_OUTPUT,
		NUM_OUTPUTS
	};

	Mult() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	}

	void processChannel(const ProcessArgs& args, int _c) override;
};

} // namespace bogaudio
