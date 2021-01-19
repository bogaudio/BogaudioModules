#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelLag;

namespace bogaudio {

struct Lag : BGModule {
	enum ParamsIds {
		TIME_PARAM,
		TIME_SCALE_PARAM,
		SHAPE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		TIME_INPUT,
		SHAPE_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	ShapedSlewLimiter _slew;

	Lag() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(TIME_PARAM, 0.0f, 1.0f, 0.5f, "time");
		configParam(TIME_SCALE_PARAM, 0.0f, 2.0f, 1.0f, "time_scale");
		configParam(SHAPE_PARAM, -1.0f, 1.0f, 0.0f, "shape");
	}

	bool active() override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
