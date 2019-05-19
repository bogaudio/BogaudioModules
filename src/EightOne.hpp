#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelEightOne;

namespace bogaudio {

struct EightOne : Module {
	enum ParamsIds {
		STEPS_PARAM,
		DIRECTION_PARAM,
		SELECT_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		IN5_INPUT,
		IN6_INPUT,
		IN7_INPUT,
		IN8_INPUT,
		CLOCK_INPUT,
		RESET_INPUT,
		SELECT_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		IN1_LIGHT,
		IN2_LIGHT,
		IN3_LIGHT,
		IN4_LIGHT,
		IN5_LIGHT,
		IN6_LIGHT,
		IN7_LIGHT,
		IN8_LIGHT,
		NUM_LIGHTS
	};

	Trigger _clock;
	Trigger _reset;
	bogaudio::dsp::Timer _timer;
	int _step;
	bool _selectOnClock = false;
	int _select = 0;

	EightOne() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(STEPS_PARAM, 1.0f, 8.0f, 8.0f, "steps");
		configParam(DIRECTION_PARAM, 0.0f, 1.0f, 1.0f, "direction");
		configParam(SELECT_PARAM, 0.0f, 7.0f, 0.0f, "select");

		onReset();
		onSampleRateChange();
	}

	void onReset() override;
	void onSampleRateChange() override;
	json_t* dataToJson() override;
	void dataFromJson(json_t* root) override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
