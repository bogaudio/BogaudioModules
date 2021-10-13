#pragma once

#include "bogaudio.hpp"
#include "dsp/envelope.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelAD;

namespace bogaudio {

struct AD : BGModule {
	enum ParamsIds {
		ATTACK_PARAM,
		DECAY_PARAM,
		LOOP_PARAM,
		LINEAR_PARAM,
		RETRIGGER_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		TRIGGER_INPUT,
		ATTACK_INPUT,
		DECAY_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		ENV_OUTPUT,
		EOC_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		ATTACK_LIGHT,
		DECAY_LIGHT,
		NUM_LIGHTS
	};

	struct Engine {
		int modulationSteps;
		Trigger trigger;
		rack::dsp::PulseGenerator eocPulseGen;
		bool on = false;
		bogaudio::dsp::ADSR envelope;
		bogaudio::dsp::SlewLimiter attackSL;
		bogaudio::dsp::SlewLimiter decaySL;

		Engine(int ms) : modulationSteps(ms) {
			reset();
			sampleRateChange();
			envelope.setSustain(0.0f);
			envelope.setRelease(0.0f);
		}
		void reset();
		void sampleRateChange();
	};
	Engine* _engines[maxChannels] {};
	bool _retriggerMode = true;
	bool _loopMode = false;
	bool _linearMode = false;
	int _attackLightSum;
	int _decayLightSum;
	float _invert = 1.0f;

	AD() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<EnvelopeSegmentParamQuantity>(ATTACK_PARAM, 0.0f, 1.0f, 0.141421f, "Attack", " s");
		configParam<EnvelopeSegmentParamQuantity>(DECAY_PARAM, 0.0f, 1.0f, 0.31623f, "Decay", " s");
		configSwitch(LOOP_PARAM, 0.0f, 1.0f, 0.0f, "Loop", {"Disabled (stop)", "Enabled"});
		configSwitch(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "Linear", {"Disabled (logarithmic)", "Enabled"});
		configSwitch(RETRIGGER_PARAM, 0.0f, 1.0f, 1.0f, "Retrigger", {"Disabled (finish cycle)", "Enabled"});

		configInput(TRIGGER_INPUT, "Trigger");
		configInput(ATTACK_INPUT, "Attack CV");
		configInput(DECAY_INPUT, "Decay CV");

		configOutput(ENV_OUTPUT, "ENV");
		configOutput(EOC_OUTPUT, "EOC");
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulateChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void postProcessAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
