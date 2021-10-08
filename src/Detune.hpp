#pragma once

#include "bogaudio.hpp"
#include "dsp/pitch.hpp"

using namespace bogaudio::dsp;

extern Model* modelDetune;

namespace bogaudio {

struct Detune : BGModule {
	enum ParamsIds {
		CENTS_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		THRU_OUTPUT,
		OUT_PLUS_OUTPUT,
		OUT_MINUS_OUTPUT,
		NUM_OUTPUTS
	};

	float _cents[maxChannels] {};
	float _lastCents[maxChannels];
	float _lastInCV[maxChannels];
	float _plusCV[maxChannels] {};
	float _minusCV[maxChannels] {};

	Detune() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(CENTS_PARAM, 0.0f, 50.0f, 0.0f, "Cents");
		paramQuantities[CENTS_PARAM]->snapEnabled = true;

		configInput(CV_INPUT, "CV");
		configInput(IN_INPUT, "IN");

		configOutput(THRU_OUTPUT, "THRU");
		configOutput(OUT_PLUS_OUTPUT, "OUT_PLUS");
		configOutput(OUT_MINUS_OUTPUT, "OUT_MINUS");

		for (int i = 0; i < maxChannels; ++i) {
			_lastCents[i] = -1.0f;
			_lastInCV[i] = -1000.0f;
		}
	}

	bool active() override;
	int channels() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void processBypass(const ProcessArgs& args) override;
};

} // namespace bogaudio
