#pragma once

#include "PEQ6_shared.hpp"

namespace bogaudio {

struct PEQ6XO : ExpanderModule<PEQ6ExpanderMessage, BGModule> {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		BAND1_OUTPUT,
		BAND2_OUTPUT,
		BAND3_OUTPUT,
		BAND4_OUTPUT,
		BAND5_OUTPUT,
		BAND6_OUTPUT,
		NUM_OUTPUTS
	};

	PEQ6XO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		setBaseModelPredicate([](Model* m) { return m == modelPEQ6; });
	}

	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
