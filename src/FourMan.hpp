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
		configButton(TRIGGER1_PARAM, "Trigger 1");
		configButton(TRIGGER2_PARAM, "Trigger 2");
		configButton(TRIGGER3_PARAM, "Trigger 3");
		configButton(TRIGGER4_PARAM, "Trigger 4");

		configOutput(OUT1_OUTPUT, "OUT1");
		configOutput(OUT2_OUTPUT, "OUT2");
		configOutput(OUT3_OUTPUT, "OUT3");
		configOutput(OUT4_OUTPUT, "OUT4");

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
