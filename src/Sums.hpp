#pragma once

#include "bogaudio.hpp"
#include "disable_output_limit.hpp"

extern Model* modelSums;

namespace bogaudio {

struct Sums : DisableOutputLimitModule {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		A_INPUT,
		B_INPUT,
		NEGATE_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		SUM_OUTPUT,
		DIFFERENCE_OUTPUT,
		MAX_OUTPUT,
		MIN_OUTPUT,
		NEGATE_OUTPUT,
		NUM_OUTPUTS
	};

	Sums() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

		configInput(A_INPUT, "Signal A");
		configInput(B_INPUT, "Signal B");
		configInput(NEGATE_INPUT, "Negative signal");

		configOutput(SUM_OUTPUT, "Sum");
		configOutput(DIFFERENCE_OUTPUT, "Difference");
		configOutput(MAX_OUTPUT, "Max");
		configOutput(MIN_OUTPUT, "Min");
		configOutput(NEGATE_OUTPUT, "Negative signal");
	}

	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
