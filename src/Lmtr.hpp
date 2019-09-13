#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelLmtr;

namespace bogaudio {

struct Lmtr : BGModule {
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

	struct Engine {
		float thresholdDb = 0.0f;
		float outGain = -1.0f;
		float outLevel = 0.0f;
		float lastEnv = 0.0f;

		bogaudio::dsp::SlewLimiter attackSL;
		bogaudio::dsp::SlewLimiter releaseSL;
		RootMeanSquare detector;
		Compressor compressor;
		Amplifier amplifier;
		Saturator saturator;
	};

	Engine* _engines[maxChannels] {};
	bool _softKnee = true;

	Lmtr() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(THRESHOLD_PARAM, 0.0f, 1.0f, 0.8f, "Threshold", " dB", 0.0f, 30.0f, -24.0f);
		configParam(OUTPUT_GAIN_PARAM, 0.0f, 1.0f, 0.0f, "Output gain", " dB", 0.0f, 24.0f);
		configParam(KNEE_PARAM, 0.0f, 1.0f, 0.0f, "Knee");

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
