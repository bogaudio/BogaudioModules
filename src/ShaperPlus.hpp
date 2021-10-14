#pragma once

#include "bogaudio.hpp"
#include "trigger_on_load.hpp"
#include "shaper_core.hpp"

extern Model* modelShaperPlus;

namespace bogaudio {

struct ShaperPlus : TriggerOnLoadModule {
	enum ParamsIds {
		ATTACK_PARAM,
		ON_PARAM,
		DECAY_PARAM,
		OFF_PARAM,
		ENV_PARAM,
		SIGNAL_PARAM,
		TRIGGER_PARAM,
		SPEED_PARAM,
		LOOP_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		SIGNAL_INPUT,
		TRIGGER_INPUT,
		ATTACK_INPUT,
		ON_INPUT,
		DECAY_INPUT,
		OFF_INPUT,
		ENV_INPUT,
		SIGNALCV_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		SIGNAL_OUTPUT,
		ENV_OUTPUT,
		INV_OUTPUT,
		TRIGGER_OUTPUT,
		ATTACK_OUTPUT,
		ON_OUTPUT,
		DECAY_OUTPUT,
		OFF_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		ATTACK_LIGHT,
		ON_LIGHT,
		DECAY_LIGHT,
		OFF_LIGHT,
		NUM_LIGHTS
	};

	ShaperCore* _core[maxChannels] {};
	float _attackLights[maxChannels] {};
	float _onLights[maxChannels] {};
	float _decayLights[maxChannels] {};
	float _offLights[maxChannels] {};

	ShaperPlus() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<EnvelopeSegmentParamQuantity>(ATTACK_PARAM, 0.0f, 1.0f, 0.14142f, "Attack", " s");
		configParam<EnvelopeSegmentParamQuantity>(ON_PARAM, 0.0f, 1.0f, 0.31623f, "On", " s");
		configParam<EnvelopeSegmentParamQuantity>(DECAY_PARAM, 0.0f, 1.0f, 0.31623f, "Decay", " s");
		configParam<EnvelopeSegmentParamQuantity>(OFF_PARAM, 0.0f, 1.0f, 0.07071f, "Off", " s");
		configParam(ENV_PARAM, 0.0f, 1.0f, 1.0f, "Env", "%", 0.0f, 100.0f);
		configParam(SIGNAL_PARAM, 0.0f, 1.0f, 0.1f, "Signal", "x", 10.0f);
		configButton(TRIGGER_PARAM, "Trigger");
		configSwitch(SPEED_PARAM, 0.0f, 1.0f, 1.0f, "Speed", {"Slow", "Normal"});
		configSwitch(LOOP_PARAM, 0.0f, 1.0f, 1.0f, "Loop", {"Loop", "Stop"});

		configInput(SIGNAL_INPUT, "Signal");
		configInput(TRIGGER_INPUT, "Trigger");
		configInput(ATTACK_INPUT, "Attack CV");
		configInput(ON_INPUT, "On CV");
		configInput(DECAY_INPUT, "Decay CV");
		configInput(OFF_INPUT, "Off CV");
		configInput(ENV_INPUT, "Envelope level CV");
		configInput(SIGNALCV_INPUT, "Output signal level CV");

		configOutput(SIGNAL_OUTPUT, "Signal");
		configOutput(ENV_OUTPUT, "Envelope");
		configOutput(INV_OUTPUT, "Inverted envelope");
		configOutput(TRIGGER_OUTPUT, "Trigger");
		configOutput(ATTACK_OUTPUT, "Attack stage gate");
		configOutput(ON_OUTPUT, "On stage gate");
		configOutput(DECAY_OUTPUT, "Decay stage gate");
		configOutput(OFF_OUTPUT, "Off stage gate");
	}

	void reset() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void postProcess(const ProcessArgs& args) override;

	bool shouldTriggerOnNextLoad() override;
};

} // namespace bogaudio
