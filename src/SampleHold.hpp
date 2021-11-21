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

	static constexpr float maxSmoothMS = 10000.0f;

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
	float _smoothMS = 0.0f;
	SlewLimiter _outputSL1[maxChannels];
	SlewLimiter _outputSL2[maxChannels];

	SampleHold() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configButton(TRIGGER1_PARAM, "Trigger 1");
		configButton(TRIGGER2_PARAM, "Trigger 2");
		configSwitch(TRACK1_PARAM, 0.0f, 1.0f, 0.0f, "Track 1", {"Disabled", "Enabled"});
		configSwitch(TRACK2_PARAM, 0.0f, 1.0f, 0.0f, "Track 2", {"Disabled", "Enabled"});
		configSwitch(INVERT1_PARAM, 0.0f, 1.0f, 0.0f, "Invert 1", {"Disabled", "Enabled"});
		configSwitch(INVERT2_PARAM, 0.0f, 1.0f, 0.0f, "Invert 2", {"Disabled", "Enabled"});

		configInput(TRIGGER1_INPUT, "Trigger 1");
		configInput(IN1_INPUT, "Signal 1");
		configInput(TRIGGER2_INPUT, "Trigger 2");
		configInput(IN2_INPUT, "Signal 2");
	}

	void reset() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	void modulateChannel(int c) override;
	void processAll(const ProcessArgs& args) override;
	void handleChannel(
		Param& trackParam,
		Param& invertParam,
		Trigger* trigger,
		Param& triggerParam,
		Input& triggerInput,
		Input* altTriggerInput,
		Input& in,
		float* value,
		SlewLimiter* _outputSL,
		Output& out
	);
	float noise();
};

} // namespace bogaudio
