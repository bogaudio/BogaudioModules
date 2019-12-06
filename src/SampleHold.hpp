#pragma once

#include "bogaudio.hpp"
#include "dsp/noise.hpp"

using namespace bogaudio::dsp;

extern Model* modelSampleHold;

namespace bogaudio {

struct SampleHold : BGModule {
	enum ParamIds {
		TRIGGER1_PARAM,
		TRIGGER2_PARAM,
		TRACK1_PARAM,
		TRACK2_PARAM,
		INVERT1_PARAM,
		INVERT2_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		TRIGGER1_INPUT,
		IN1_INPUT,
		TRIGGER2_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};

	enum OutputIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	enum NoiseType {
		WHITE_NOISE_TYPE,
		BLUE_NOISE_TYPE,
		PINK_NOISE_TYPE,
		RED_NOISE_TYPE
	};

	Trigger _trigger1[maxChannels];
	Trigger _trigger2[maxChannels];
	float _value1[maxChannels] {};
	float _value2[maxChannels] {};
	BlueNoiseGenerator _blue;
	WhiteNoiseGenerator _white;
	PinkNoiseGenerator _pink;
	RedNoiseGenerator _red;
	NoiseType _noiseType = WHITE_NOISE_TYPE;
	float _rangeOffset = 1.0f;
	float _rangeScale = 5.0f;
	int _polyInputID = TRIGGER1_INPUT;

	SampleHold() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(TRIGGER1_PARAM, 0.0f, 10.0f, 0.0f, "Trigger 1");
		configParam(TRIGGER2_PARAM, 0.0f, 10.0f, 0.0f, "Trigger 2");
		configParam(TRACK1_PARAM, 0.0f, 1.0f, 0.0f, "Track 1");
		configParam(TRACK2_PARAM, 0.0f, 1.0f, 0.0f, "Track 2");
		configParam(INVERT1_PARAM, 0.0f, 1.0f, 0.0f, "Invert 1");
		configParam(INVERT2_PARAM, 0.0f, 1.0f, 0.0f, "Invert 2");

		reset();
	}

	void reset() override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void processChannel(
		Param& trackParam,
		Param& invertParam,
		Trigger* trigger,
		Param& triggerParam,
		Input& triggerInput,
		Input* altTriggerInput,
		Input& in,
		float* value,
		Output& out
	);
	float noise();
};

} // namespace bogaudio
