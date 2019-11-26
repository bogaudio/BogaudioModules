#pragma once

#include "bogaudio.hpp"

extern Model* modelUnison;

namespace bogaudio {

struct Unison : BGModule {
	enum ParamsIds {
		CHANNELS_PARAM,
		DETUNE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		DETUNE_INPUT,
		PITCH_INPUT,
		GATE_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		PITCH_OUTPUT,
		GATE_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	const float maxDetuneCents = 50.0f;
	int _channels = 0;
	float _cents = 0.0f;

	Unison() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(CHANNELS_PARAM, 1.0f, 16.0f, 1.0f, "Channels");
		configParam(DETUNE_PARAM, 0.0f, maxDetuneCents, 0.0f, "Detune");
		onReset();
	}

	void modulate() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
