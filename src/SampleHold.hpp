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

	enum LightsIds {
		TRACK1_LIGHT,
		TRACK2_LIGHT,
		NUM_LIGHTS
	};

	enum NoiseType {
		WHITE_NOISE_TYPE,
		BLUE_NOISE_TYPE,
		PINK_NOISE_TYPE,
		RED_NOISE_TYPE
	};

	Trigger _trigger1, _trigger2;
	float _value1, _value2;
	BlueNoiseGenerator _blue;
	WhiteNoiseGenerator _white;
	PinkNoiseGenerator _pink;
	RedNoiseGenerator _red;
	NoiseType _noiseType = WHITE_NOISE_TYPE;
	float _rangeOffset = 1.0f;
	float _rangeScale = 5.0f;

	SampleHold()
	:  _value1(0.0f)
	, _value2(0.0f)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(TRIGGER1_PARAM, 0.0f, 10.0f, 0.0f, "Trigger 1");
		configParam(TRIGGER2_PARAM, 0.0f, 10.0f, 0.0f, "Trigger 2");
		configParam(TRACK1_PARAM, 0.0f, 1.0f, 0.0f, "Track 1");
		configParam(TRACK2_PARAM, 0.0f, 1.0f, 0.0f, "Track 2");

		onReset();
	}

	void onReset() override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void process(const ProcessArgs& args) override;
	void processChannel(
		Light& trackLight,
		Param& trackParam,
		Trigger& trigger,
		Param& triggerParam,
		Input& triggerInput,
		Input& in,
		float& value,
		Output& out
	);
	float noise();
};

} // namespace bogaudio
