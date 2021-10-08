#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/utility.hpp"
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

		void sampleRateChange();
	};

	Engine* _engines[maxChannels] {};
	float _compressionDb = 0.0f;
	bool _compressorMode = true;
	bool _rmsDetector = true;
	bool _softKnee = true;
	float _thresholdRange = 1.0f;

	struct ThresholdParamQuantity : ParamQuantity {
		float getDisplayValue() override;
		void setDisplayValue(float v) override;
	};

	Pressor() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<ThresholdParamQuantity>(THRESHOLD_PARAM, 0.0f, 1.0f, 0.8f, "Threshold", " dB");
		configParam<DynamicsRatioParamQuantity>(RATIO_PARAM, 0.0f, 1.0f, 0.55159f, "Ratio");
		configParam<ScaledSquaringParamQuantity<500>>(ATTACK_PARAM, 0.0f, 1.0f, 0.31623f, "Attack", " ms");
		configParam<ScaledSquaringParamQuantity<2>>(RELEASE_PARAM, 0.0f, 1.0f, 0.31623f, "Release", " s");
		configParam(OUTPUT_GAIN_PARAM, 0.0f, 1.0f, 0.0f, "Output gain", " dB", 0.0f, 24.0f);
		configParam(INPUT_GAIN_PARAM, -1.0f, 1.0f, 0.0f, "Input gain", " dB", 0.0f, 12.0f);
		configParam(DETECTOR_MIX_PARAM, -1.0f, 1.0f, 0.0f, "Detector mix", "%", 0.0f, 100.0f);
		configSwitch(MODE_PARAM, 0.0f, 1.0f, 1.0f, "Mode", {"Noise gate", "Compressor"});
		configSwitch(DECTECTOR_MODE_PARAM, 0.0f, 1.0f, 1.0f, "Dectector mode", {"Peak", "RMS"});
		configSwitch(KNEE_PARAM, 0.0f, 1.0f, 1.0f, "Knee", {"Hard", "Soft"});
		configBypass(LEFT_INPUT, LEFT_OUTPUT);
		configBypass(RIGHT_INPUT, RIGHT_OUTPUT);

		configInput(LEFT_INPUT, "LEFT");
		configInput(SIDECHAIN_INPUT, "SIDECHAIN");
		configInput(THRESHOLD_INPUT, "THRESHOLD");
		configInput(RATIO_INPUT, "RATIO");
		configInput(RIGHT_INPUT, "RIGHT");
		configInput(ATTACK_INPUT, "ATTACK");
		configInput(RELEASE_INPUT, "RELEASE");
		configInput(INPUT_GAIN_INPUT, "INPUT_GAIN");
		configInput(OUTPUT_GAIN_INPUT, "OUTPUT_GAIN");

		configOutput(ENVELOPE_OUTPUT, "ENVELOPE");
		configOutput(LEFT_OUTPUT, "LEFT");
		configOutput(RIGHT_OUTPUT, "RIGHT");
	}

	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
