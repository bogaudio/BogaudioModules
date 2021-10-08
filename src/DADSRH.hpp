#pragma once

#include "bogaudio.hpp"
#include "trigger_on_load.hpp"
#include "dadsrh_core.hpp"

extern Model* modelDADSRH;

namespace bogaudio {

struct DADSRH : TriggerOnLoadModule {
	enum ParamsIds {
		DELAY_PARAM,
		ATTACK_PARAM,
		DECAY_PARAM,
		SUSTAIN_PARAM,
		RELEASE_PARAM,
		HOLD_PARAM,
		ATTACK_SHAPE_PARAM,
		DECAY_SHAPE_PARAM,
		RELEASE_SHAPE_PARAM,
		TRIGGER_PARAM,
		MODE_PARAM,
		LOOP_PARAM,
		SPEED_PARAM,
		RETRIGGER_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		TRIGGER_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		ENV_OUTPUT,
		INV_OUTPUT,
		TRIGGER_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		DELAY_LIGHT,
		ATTACK_LIGHT,
		DECAY_LIGHT,
		SUSTAIN_LIGHT,
		RELEASE_LIGHT,
		ATTACK_SHAPE1_LIGHT,
		ATTACK_SHAPE2_LIGHT,
		ATTACK_SHAPE3_LIGHT,
		DECAY_SHAPE1_LIGHT,
		DECAY_SHAPE2_LIGHT,
		DECAY_SHAPE3_LIGHT,
		RELEASE_SHAPE1_LIGHT,
		RELEASE_SHAPE2_LIGHT,
		RELEASE_SHAPE3_LIGHT,
		NUM_LIGHTS
	};

	DADSRHCore* _core[maxChannels] {};
	float _delayLights[maxChannels] {};
	float _attackLights[maxChannels] {};
	float _decayLights[maxChannels] {};
	float _sustainLights[maxChannels] {};
	float _releaseLights[maxChannels] {};

	DADSRH() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<EnvelopeSegmentParamQuantity>(DELAY_PARAM, 0.0f, 1.0f, 0.0f, "Delay", " s");
		configParam<EnvelopeSegmentParamQuantity>(ATTACK_PARAM, 0.0f, 1.0f, 0.14142f, "Attack", " s");
		configParam<EnvelopeSegmentParamQuantity>(DECAY_PARAM, 0.0f, 1.0f, 0.31623f, "Decay", " s");
		configParam(SUSTAIN_PARAM, 0.0f, 1.0f, 0.5f, "Sustain", "%", 0.0f, 100.0f);
		configParam<EnvelopeSegmentParamQuantity>(RELEASE_PARAM, 0.0f, 1.0f, 0.31623f, "Release", " s");
		configParam<EnvelopeSegmentParamQuantity>(HOLD_PARAM, 0.0f, 1.0f, 0.44721f, "Hold", " s");
		configSwitch(ATTACK_SHAPE_PARAM, 1.0f, 3.0f, 1.0f, "Attack shape", {"Logarithmic", "Linear", "Exponential"});
		configSwitch(DECAY_SHAPE_PARAM, 1.0f, 3.0f, 1.0f, "Decay shape", {"Exponential", "Linear", "Logarithmic"});
		configSwitch(RELEASE_SHAPE_PARAM, 1.0f, 3.0f, 1.0f, "Release shape", {"Exponential", "Linear", "Logarithmic"});
		configButton(TRIGGER_PARAM, "Trigger");
		configSwitch(MODE_PARAM, 0.0f, 1.0f, 1.0f, "Mode", {"Triggered", "Gated"});
		configSwitch(LOOP_PARAM, 0.0f, 1.0f, 1.0f, "Loop", {"Loop", "Stop"});
		configSwitch(SPEED_PARAM, 0.0f, 1.0f, 1.0f, "Speed", {"Slow", "Normal"});
		configSwitch(RETRIGGER_PARAM, 0.0f, 1.0f, 1.0f, "Retrigger", {"Reset", "Resume attack"});

		configInput(TRIGGER_INPUT, "TRIGGER");

		configOutput(ENV_OUTPUT, "ENV");
		configOutput(INV_OUTPUT, "INV");
		configOutput(TRIGGER_OUTPUT, "TRIGGER");
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
