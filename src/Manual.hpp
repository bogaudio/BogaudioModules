#pragma once

#include "bogaudio.hpp"
#include "trigger_on_load.hpp"

extern Model* modelManual;

namespace bogaudio {

struct Manual : TriggerOnLoadModule {
	enum ParamsIds {
		TRIGGER_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
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
		NUM_LIGHTS
	};

	bool _firstStep = true;
	Trigger _trigger;
	rack::dsp::PulseGenerator _pulse;

	Manual() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(TRIGGER_PARAM, 0.0f, 1.0f, 0.0f, "Trigger");
		_triggerOnLoad = false;
		onReset();
	}

	void onReset() override;
	void process(const ProcessArgs& args) override;
	bool shouldTriggerOnNextLoad() override {
		return true;
	}
};

} // namespace bogaudio
