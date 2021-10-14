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

		configInput(INA_INPUT, "Signal A");
		configInput(INB_INPUT, "Signal B");

		configOutput(OUTA1_OUTPUT, "Signal A");
		configOutput(OUTA2_OUTPUT, "Signal A");
		configOutput(OUTA3_OUTPUT, "Signal A");
		configOutput(OUTB1_OUTPUT, "Signal B");
		configOutput(OUTB2_OUTPUT, "Signal B");
		configOutput(OUTB3_OUTPUT, "Signal B");
	}

	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
