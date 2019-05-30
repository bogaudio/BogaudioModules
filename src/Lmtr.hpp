#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelLmtr;

namespace bogaudio {

struct Lmtr : Module {
	enum ParamsIds {
		THRESHOLD_PARAM,
		OUTPUT_GAIN_PARAM,
		KNEE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		LEFT_INPUT,
		RIGHT_INPUT,
		THRESHOLD_INPUT,
		OUTPUT_GAIN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	const int modulationSteps = 100;
	int _modulationStep = 0;
	float _thresholdDb = 0.0f;
	float _outGain = -1.0f;
	float _outLevel = 0.0f;
	bool _softKnee = true;
	float _lastEnv = 0.0f;

	bogaudio::dsp::SlewLimiter _attackSL;
	bogaudio::dsp::SlewLimiter _releaseSL;
	RootMeanSquare _detector;
	Compressor _compressor;
	Amplifier _amplifier;
	Saturator _saturator;

	Lmtr() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(THRESHOLD_PARAM, 0.0f, 1.0f, 0.8f, "threshold");
		configParam(OUTPUT_GAIN_PARAM, 0.0f, 1.0f, 0.0f, "output_gain");
		configParam(KNEE_PARAM, 0.0f, 1.0f, 0.0f, "knee");

		onReset();
		onSampleRateChange();
	}

	void onReset() override;
	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
