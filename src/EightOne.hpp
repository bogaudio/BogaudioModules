#pragma once

#include "bogaudio.hpp"
#include "select_on_clock.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelEightOne;

namespace bogaudio {

struct EightOne : SelectOnClockModule {
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

	Trigger _clock[maxChannels];
	Trigger _reset[maxChannels];
	bogaudio::dsp::Timer _timer[maxChannels];
	int _step[maxChannels];
	float _select[maxChannels] {};

	EightOne() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(STEPS_PARAM, 1.0f, 8.0f, 8.0f, "Steps");
		configParam(DIRECTION_PARAM, 0.0f, 1.0f, 1.0f, "Direction");
		configParam(SELECT_PARAM, 0.0f, 7.0f, 0.0f, "Select step");

		reset();
		sampleRateChange();
	}

	void reset() override;
	void sampleRateChange() override;
	int channels() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
