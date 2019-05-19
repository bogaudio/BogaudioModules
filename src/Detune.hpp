#pragma once

#include "bogaudio.hpp"
#include "dsp/pitch.hpp"

using namespace bogaudio::dsp;

extern Model* modelDetune;

namespace bogaudio {

struct Detune : Module {
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

	enum LightsIds {
		NUM_LIGHTS
	};

	float _cents = -1.0f;
	float _inCV = -1000.0f;
	float _plusCV;
	float _minusCV;

	Detune() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CENTS_PARAM, 0.0f, 50.0f, 0.0f, "cents");
	}

	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
