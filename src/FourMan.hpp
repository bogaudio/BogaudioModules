#pragma once

#include "bogaudio.hpp"
#include "trigger_on_load.hpp"
#include "dsp/signal.hpp"

extern Model* modelFourMan;

namespace bogaudio {

struct FourMan : TriggerOnLoadModule {
	enum ParamsIds {
		TRIGGER1_PARAM,
		TRIGGER2_PARAM,
		TRIGGER3_PARAM,
		TRIGGER4_PARAM,
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
		NUM_OUTPUTS
	};

	Trigger _trigger[4];
	rack::dsp::PulseGenerator _pulse[4];
	float _sampleTime = 1.0f;
	bogaudio::dsp::Timer* _initialDelay = NULL;
	float _outputScale = 1.0f;

	FourMan() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(TRIGGER1_PARAM, 0.0f, 1.0f, 0.0f, "Trigger 1");
		configParam(TRIGGER2_PARAM, 0.0f, 1.0f, 0.0f, "Trigger 2");
		configParam(TRIGGER3_PARAM, 0.0f, 1.0f, 0.0f, "Trigger 3");
		configParam(TRIGGER4_PARAM, 0.0f, 1.0f, 0.0f, "Trigger 4");
		_triggerOnLoad = false;
		_initialDelay = new bogaudio::dsp::Timer(APP->engine->getSampleRate(), 0.01f);
	}
	virtual ~FourMan() {
		if (_initialDelay) {
			delete _initialDelay;
		}
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	void processAll(const ProcessArgs& args) override;
	bool shouldTriggerOnNextLoad() override {
		return true;
	}
};

} // namespace bogaudio
