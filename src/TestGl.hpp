#pragma once

#include "bogaudio.hpp"

extern Model* modelTestGl;

namespace bogaudio {

struct TestGl : BGModule {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		NUM_INPUTS
	};

	enum OutputsIds {
		NUM_OUTPUTS
	};

	TestGl() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

	}

	void reset() override;
	void sampleRateChange() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
