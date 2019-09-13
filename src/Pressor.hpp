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

	struct Engine {
		float thresholdDb = 0.0f;
		float ratio = 0.0f;
		float ratioKnob = -1.0f;
		float inGain = -1.0f;
		float inLevel = 0.0f;
		float outGain = -1.0f;
		float outLevel = 0.0f;
		float lastEnv = 0.0f;

		bogaudio::dsp::SlewLimiter attackSL;
		bogaudio::dsp::SlewLimiter releaseSL;
		CrossFader detectorMix;
		RootMeanSquare detectorRMS;
		Compressor compressor;
		NoiseGate noiseGate;
		Amplifier amplifier;
		Saturator saturator;

		Engine() : detectorRMS(1000.0f, 1.0f, 50.0f) {}
	};

	Engine* _engines[maxChannels] {};
	float _compressionDb = 0.0f;
	bool _compressorMode = true;
	bool _rmsDetector = true;
	bool _softKnee = true;

	Pressor() {
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
	int channels() override;
	void addEngine(int c) override;
	void removeEngine(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
