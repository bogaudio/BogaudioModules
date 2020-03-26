#pragma once

#include "bogaudio.hpp"
#include "trigger_on_load.hpp"
#include "dsp/signal.hpp"

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

	Trigger _trigger;
	rack::dsp::PulseGenerator _pulse;
	bogaudio::dsp::Timer* _initialDelay = NULL;

	Manual() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(TRIGGER_PARAM, 0.0f, 1.0f, 0.0f, "Trigger");
		_triggerOnLoad = false;
		_initialDelay = new bogaudio::dsp::Timer(APP->engine->getSampleRate(), 0.01f);
	}
	virtual ~Manual() {
		if (_initialDelay) {
			delete _initialDelay;
		}
	}

	void reset() override;
	void processChannel(const ProcessArgs& args, int _c) override;
	bool shouldTriggerOnNextLoad() override {
		return true;
	}
};

} // namespace bogaudio
