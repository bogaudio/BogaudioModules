#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelSlew;

namespace bogaudio {

struct Slew : Module {
	enum ParamsIds {
		RISE_PARAM,
		RISE_SHAPE_PARAM,
		FALL_PARAM,
		FALL_SHAPE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		RISE_INPUT,
		FALL_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	const int modulationSteps = 100;
	int _modulationStep = 0;
	bool _rising = true;
	float _last = 0.0f;
	ShapedSlewLimiter _rise;
	ShapedSlewLimiter _fall;

	Slew() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RISE_PARAM, 0.0f, 1.0f, 0.316f, "rise");
		configParam(RISE_SHAPE_PARAM, -1.0f, 1.0f, 0.0f, "rise_shape");
		configParam(FALL_PARAM, 0.0f, 1.0f, 0.316f, "fall");
		configParam(FALL_SHAPE_PARAM, -1.0f, 1.0f, 0.0f, "fall_shape");

		onReset();
	}

	void onReset() override;
	void process(const ProcessArgs& args) override;
	float time(Param& param, Input& input);
	float shape(Param& param);
};

} // namespace bogaudio
