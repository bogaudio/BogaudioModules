#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelPressor;

namespace bogaudio {

struct Pressor : Module {
	enum ParamsIds {
		THRESHOLD_PARAM,
		RATIO_PARAM,
		ATTACK_PARAM,
		RELEASE_PARAM,
		OUTPUT_GAIN_PARAM,
		INPUT_GAIN_PARAM,
		DETECTOR_MIX_PARAM,
		MODE_PARAM,
		DECTECTOR_MODE_PARAM,
		KNEE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		LEFT_INPUT,
		SIDECHAIN_INPUT,
		THRESHOLD_INPUT,
		RATIO_INPUT,
		RIGHT_INPUT,
		ATTACK_INPUT,
		RELEASE_INPUT,
		INPUT_GAIN_INPUT,
		OUTPUT_GAIN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		ENVELOPE_OUTPUT,
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
	float _ratio = 0.0f;
	float _ratioKnob = -1.0f;
	float _inGain = -1.0f;
	float _inLevel = 0.0f;
	float _outGain = -1.0f;
	float _outLevel = 0.0f;
	bool _compressorMode = true;
	bool _rmsDetector = true;
	bool _softKnee = true;
	float _lastEnv = 0.0f;
	float _compressionDb = 0.0f;

	bogaudio::dsp::SlewLimiter _attackSL;
	bogaudio::dsp::SlewLimiter _releaseSL;
	CrossFader _detectorMix;
	RootMeanSquare _detectorRMS;
	Compressor _compressor;
	NoiseGate _noiseGate;
	Amplifier _amplifier;
	Saturator _saturator;

	Pressor()
	:  _detectorRMS(1000.0f, 1.0f, 50.0f)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(THRESHOLD_PARAM, 0.0f, 1.0f, 0.8f, "threshold");
		configParam(RATIO_PARAM, 0.0f, 1.0f, 0.552f, "ratio");
		configParam(ATTACK_PARAM, 0.0f, 1.0f, 0.316f, "attack");
		configParam(RELEASE_PARAM, 0.0f, 1.0f, 0.316f, "release");
		configParam(OUTPUT_GAIN_PARAM, 0.0f, 1.0f, 0.0f, "output_gain");
		configParam(INPUT_GAIN_PARAM, -1.0f, 1.0f, 0.0f, "input_gain");
		configParam(DETECTOR_MIX_PARAM, -1.0f, 1.0f, 0.0f, "detector_mix");
		configParam(MODE_PARAM, 0.0f, 1.0f, 1.0f, "mode");
		configParam(DECTECTOR_MODE_PARAM, 0.0f, 1.0f, 1.0f, "dectector_mode");
		configParam(KNEE_PARAM, 0.95f, 1.0f, 1.0f, "knee");

		onReset();
		onSampleRateChange();
	}

	void onReset() override;
	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
