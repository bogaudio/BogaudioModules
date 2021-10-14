#pragma once

#include "bogaudio.hpp"
#include "dsp/envelope.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelASR;

namespace bogaudio {

struct ASR : BGModule {
	enum ParamsIds {
		ATTACK_PARAM,
		RELEASE_PARAM,
		SUSTAIN_PARAM,
		LINEAR_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		TRIGGER_INPUT,
		ATTACK_INPUT,
		RELEASE_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		ENV_OUTPUT,
		EOC_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		ATTACK_LIGHT,
		RELEASE_LIGHT,
		NUM_LIGHTS
	};

	struct Engine {
		int modulationSteps;
		Trigger trigger;
		rack::dsp::PulseGenerator eocPulseGen;
		bool on = false;
		bogaudio::dsp::ADSR envelope;
		bogaudio::dsp::SlewLimiter attackSL;
		bogaudio::dsp::SlewLimiter releaseSL;

		Engine(int ms) : modulationSteps(ms) {
			reset();
			sampleRateChange();
			envelope.setDecay(0.0f);
		}
		void reset();
		void sampleRateChange();
	};
	Engine* _engines[maxChannels] {};
	bool _linearMode = false;
	int _attackLightSum;
	int _releaseLightSum;
	float _invert = 1.0f;

	ASR() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<EnvelopeSegmentParamQuantity>(ATTACK_PARAM, 0.0f, 1.0f, 0.141421f, "Attack", " s");
		configParam<EnvelopeSegmentParamQuantity>(RELEASE_PARAM, 0.0f, 1.0f, 0.31623f, "Release", " s");
		configParam(SUSTAIN_PARAM, 0.0f, 1.0f, 1.0f, "Sustain", "", 0.0f, 10.0f);
		configSwitch(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "Linear", {"Disabled (logarithmic)", "Enabled"});

		configInput(TRIGGER_INPUT, "Trigger");
		configInput(ATTACK_INPUT, "Attack CV");
		configInput(RELEASE_INPUT, "Release CV");

		configOutput(ENV_OUTPUT, "Envelope");
		configOutput(EOC_OUTPUT, "End-of-cycle trigger");
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
