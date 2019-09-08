#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelLmtr;

namespace bogaudio {

struct Lmtr : ModulatingBGModule {
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
		configParam(THRESHOLD_PARAM, 0.0f, 1.0f, 0.8f, "Threshold", " dB", 0.0f, 30.0f, -24.0f);
		configParam(OUTPUT_GAIN_PARAM, 0.0f, 1.0f, 0.0f, "Output gain", " dB", 0.0f, 24.0f);
		configParam(KNEE_PARAM, 0.0f, 1.0f, 0.0f, "Knee");

		sampleRateChange();
	}

	void sampleRateChange() override;
	bool active() override;
	void modulate() override;
	void processIfActive(const ProcessArgs& args) override;
};

} // namespace bogaudio
