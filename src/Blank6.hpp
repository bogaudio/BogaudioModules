#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/utility.hpp"

using namespace bogaudio::dsp;

extern Model* modelBlank6;

namespace bogaudio {

struct Blank6 : BGModule {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		NUM_OUTPUTS
	};

	RootMeanSquare _rms;
	bool _haveLevel = false;
	float _level = 0.0f;

	Blank6() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

		configInput(IN_INPUT, "IN");

		sampleRateChange();
	}

	void sampleRateChange() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
