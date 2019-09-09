#pragma once

#include "bogaudio.hpp"
#include "select_on_clock.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelOneEight;

namespace bogaudio {

struct OneEight : SelectOnClockModule {
	enum ParamsIds {
		STEPS_PARAM,
		DIRECTION_PARAM,
		SELECT_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CLOCK_INPUT,
		RESET_INPUT,
		SELECT_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		OUT7_OUTPUT,
		OUT8_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		OUT1_LIGHT,
		OUT2_LIGHT,
		OUT3_LIGHT,
		OUT4_LIGHT,
		OUT5_LIGHT,
		OUT6_LIGHT,
		OUT7_LIGHT,
		OUT8_LIGHT,
		NUM_LIGHTS
	};

	Trigger _clock;
	Trigger _reset;
	bogaudio::dsp::Timer _timer;
	int _step;
	float _select = 0.0f;

	OneEight() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(STEPS_PARAM, 1.0f, 8.0f, 8.0f, "Steps");
		configParam(DIRECTION_PARAM, 0.0f, 1.0f, 1.0f, "Direction");
		configParam(SELECT_PARAM, 0.0f, 7.0f, 0.0f, "Select step");

		reset();
		sampleRateChange();
	}

	void reset() override;
	void sampleRateChange() override;
	void processChannel(const ProcessArgs& args, int _c) override;
};

} // namespace bogaudio
