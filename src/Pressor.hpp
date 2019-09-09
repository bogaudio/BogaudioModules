#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelPressor;

namespace bogaudio {

struct Pressor : BGModule {
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
		configParam(THRESHOLD_PARAM, 0.0f, 1.0f, 0.8f, "Threshold", " dB", 0.0f, 30.0f, -24.0f);
		configParam<DynamicsRatioParamQuantity>(RATIO_PARAM, 0.0f, 1.0f, 0.55159f, "Ratio");
		configParam<ScaledSquaringParamQuantity<500>>(ATTACK_PARAM, 0.0f, 1.0f, 0.31623f, "Attack", " ms");
		configParam<ScaledSquaringParamQuantity<2>>(RELEASE_PARAM, 0.0f, 1.0f, 0.31623f, "Release", " s");
		configParam(OUTPUT_GAIN_PARAM, 0.0f, 1.0f, 0.0f, "Output gain", " dB", 0.0f, 24.0f);
		configParam(INPUT_GAIN_PARAM, -1.0f, 1.0f, 0.0f, "Input gain", " dB", 0.0f, 12.0f);
		configParam(DETECTOR_MIX_PARAM, -1.0f, 1.0f, 0.0f, "Detector mix", "%", 0.0f, 100.0f);
		configParam(MODE_PARAM, 0.0f, 1.0f, 1.0f, "Mode");
		configParam(DECTECTOR_MODE_PARAM, 0.0f, 1.0f, 1.0f, "Dectector mode");
		configParam(KNEE_PARAM, 0.0f, 1.0f, 1.0f, "Knee");

		sampleRateChange();
	}

	void sampleRateChange() override;
	bool active() override;
	void modulate() override;
	void processChannel(const ProcessArgs& args, int _c) override;
};

} // namespace bogaudio
