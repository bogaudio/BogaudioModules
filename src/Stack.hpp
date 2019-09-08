#pragma once

#include "bogaudio.hpp"
#include "dsp/pitch.hpp"

using namespace bogaudio::dsp;

extern Model* modelStack;

namespace bogaudio {

struct Stack : BGModule {
	enum ParamsIds {
		SEMIS_PARAM,
		OCTAVE_PARAM,
		FINE_PARAM,
		QUANTIZE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		THRU_OUTPUT,
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		QUANTIZE_LIGHT,
		NUM_LIGHTS
	};

	const float _minCVOut = semitoneToCV(24.0); // C1
	const float _maxCVOut = semitoneToCV(120.0); // C9

	float _semitones = -1000.0f;
	float _inCV = -1000.0f;
	float _fine = -1000.0f;
	float _outCV;

	Stack() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(SEMIS_PARAM, 0.0f, 11.0f, 0.0f, "Semitones");
		configParam(OCTAVE_PARAM, -3.0f, 3.0f, 0.0f, "Octaves");
		configParam(FINE_PARAM, -0.99f, 0.99f, 0.0f, "Fine tune", " cents", 0.0f, 100.0f);
		configParam(QUANTIZE_PARAM, 0.0f, 1.0f, 1.0f, "Quantize");
	}

	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
