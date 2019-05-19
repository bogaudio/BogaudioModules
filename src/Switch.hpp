#pragma once

#include "bogaudio.hpp"

extern Model* modelSwitch;

namespace bogaudio {

struct Switch : Module {
	enum ParamsIds {
		GATE_PARAM,
		LATCH_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		GATE_INPUT,
		HIGH1_INPUT,
		LOW1_INPUT,
		HIGH2_INPUT,
		LOW2_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		LATCH_LIGHT,
		NUM_LIGHTS
	};

	Trigger _trigger;
	bool _latchedHigh = false;

	Switch() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(GATE_PARAM, 0.0f, 10.0f, 0.0f, "gate");
		configParam(LATCH_PARAM, 0.0f, 1.0f, 0.0f, "latch");
		onReset();
	}

	void onReset() override;
	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
