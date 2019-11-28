#pragma once

#include "bogaudio.hpp"
#include "trigger_on_load.hpp"
#include "shaper_core.hpp"

extern Model* modelShaper;

namespace bogaudio {

struct Shaper : TriggerOnLoadModule {
	enum ParamIds {
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

	enum InputIds {
		SIGNAL_INPUT,
		TRIGGER_INPUT,
		NUM_INPUTS
	};

	enum OutputIds {
		SIGNAL_OUTPUT,
		ENV_OUTPUT,
		INV_OUTPUT,
		TRIGGER_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightIds {
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

	Shaper() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<EnvelopeSegmentParamQuantity>(ATTACK_PARAM, 0.0f, 1.0f, 0.14142f, "Attack", " s");
		configParam<EnvelopeSegmentParamQuantity>(ON_PARAM, 0.0f, 1.0f, 0.31623f, "On", " s");
		configParam<EnvelopeSegmentParamQuantity>(DECAY_PARAM, 0.0f, 1.0f, 0.31623f, "Decay", " s");
		configParam<EnvelopeSegmentParamQuantity>(OFF_PARAM, 0.0f, 1.0f, 0.07071f, "Off", " s");
		configParam(ENV_PARAM, 0.0f, 1.0f, 1.0f, "Env", "%", 0.0f, 100.0f);
		configParam(SIGNAL_PARAM, 0.0f, 1.0f, 0.1f, "Signal", "x", 10.0f);
		configParam(TRIGGER_PARAM, 0.0f, 1.0f, 0.0f, "Trigger");
		configParam(SPEED_PARAM, 0.0f, 1.0f, 1.0f, "Speed");
		configParam(LOOP_PARAM, 0.0f, 1.0f, 1.0f, "Loop");
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
