#pragma once

#include "bogaudio.hpp"

extern Model* modelSwitch;

namespace bogaudio {

struct Switch : BGModule {
	enum ParamsIds {
		GATE_PARAM,
		LATCH_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		GATE_INPUT,
		HIGH1_INPUT,
		LOW1_INPUT,
		HIGH2_INPUT,
		LOW2_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		LATCH_LIGHT,
		NUM_LIGHTS
	};

	Trigger _trigger[maxChannels];
	bool _latchedHigh[maxChannels] {};
	bool _latch = false;

	Switch() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(GATE_PARAM, 0.0f, 10.0f, 0.0f, "Gate");
		configParam(LATCH_PARAM, 0.0f, 1.0f, 0.0f, "Latch");
		reset();
	}

	void reset() override;
	bool active() override;
	int channels() override;
	void channelsChanged(int before, int after) override;
	void modulate() override;
	void always(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int _c) override;
};

} // namespace bogaudio
