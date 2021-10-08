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

		configInput(INA_INPUT, "INA");
		configInput(INB_INPUT, "INB");

		configOutput(OUTA1_OUTPUT, "OUTA1");
		configOutput(OUTA2_OUTPUT, "OUTA2");
		configOutput(OUTA3_OUTPUT, "OUTA3");
		configOutput(OUTB1_OUTPUT, "OUTB1");
		configOutput(OUTB2_OUTPUT, "OUTB2");
		configOutput(OUTB3_OUTPUT, "OUTB3");
	}

	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
