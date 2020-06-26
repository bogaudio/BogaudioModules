#pragma once

#include "PEQ14_shared.hpp"

namespace bogaudio {

struct PEQ14XO : ExpanderModule<PEQ14ExpanderMessage, BGModule> {
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
		BAND7_OUTPUT,
		BAND8_OUTPUT,
		BAND9_OUTPUT,
		BAND10_OUTPUT,
		BAND11_OUTPUT,
		BAND12_OUTPUT,
		BAND13_OUTPUT,
		BAND14_OUTPUT,
		NUM_OUTPUTS
	};

	PEQ14XO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		setBaseModel(modelPEQ14);
	}

	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
