#pragma once

#include "bogaudio.hpp"

extern Model* modelInv;

namespace bogaudio {

struct Inv : BGModule {
	enum ParamsIds {
		GATE1_PARAM,
		LATCH1_PARAM,
		GATE2_PARAM,
		LATCH2_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		GATE1_INPUT,
		IN1_INPUT,
		GATE2_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		LOW1_LIGHT,
		HIGH1_LIGHT,
		LOW2_LIGHT,
		HIGH2_LIGHT,
		NUM_LIGHTS
	};

	bool _saveLatchedToPatch = false;
	Trigger _trigger[2][maxChannels];
	bool _latch[2] {};
	bool _latchedHigh[2][maxChannels] {{},{}};

	Inv() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(GATE1_PARAM, 0.0f, 1.0f, 0.0f, "Channel 1 gate");
		configSwitch(LATCH1_PARAM, 0.0f, 1.0f, 0.0f, "Channel 1 latch", {"Disabled", "Enabled"});
		configParam(GATE2_PARAM, 0.0f, 1.0f, 0.0f, "Channel 2 gate");
		configSwitch(LATCH2_PARAM, 0.0f, 1.0f, 0.0f, "Channle 2 latch", {"Disabled", "Enabled"});
		configBypass(IN1_INPUT, OUT1_OUTPUT);
		configBypass(IN2_INPUT, OUT2_OUTPUT);

		configInput(GATE1_INPUT, "GATE1");
		configInput(IN1_INPUT, "IN1");
		configInput(GATE2_INPUT, "GATE2");
		configInput(IN2_INPUT, "IN2");

		configOutput(OUT1_OUTPUT, "OUT1");
		configOutput(OUT2_OUTPUT, "OUT2");
	}

	void reset() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
	void processDual(int i);
};

} // namespace bogaudio
