#pragma once

#include "bogaudio.hpp"
#include "save_latch_to_patch.hpp"

extern Model* modelLgsw;

namespace bogaudio {

struct Lgsw : SaveLatchToPatchModule {
	enum ParamsIds {
		GATE_PARAM,
		LATCH_PARAM,
		LOGIC_MODE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		GATE_A_INPUT,
		GATE_B_INPUT,
		LOGIC_MODE_INPUT,
		HIGH_INPUT,
		LOW_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		LOGIC_OR_LIGHT,
		LOGIC_AND_LIGHT,
		LOGIC_XOR_LIGHT,
		LOGIC_NOR_LIGHT,
		LOGIC_NAND_LIGHT,
		HIGH_LIGHT,
		LOW_LIGHT,
		NUM_LIGHTS
	};

	enum Logic {
		OR_LOGIC,
		AND_LOGIC,
		XOR_LOGIC,
		NOR_LOGIC,
		NAND_LOGIC
	};

	Trigger _buttonTriggers[maxChannels];
	Trigger _aTriggers[maxChannels];
	Trigger _bTriggers[maxChannels];
	bool _lastLogicTrue[maxChannels] {};
	int _highLightSum = 0;
	int _lowLightSum = 0;
	Logic _logic = OR_LOGIC;

	Lgsw() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configButton(GATE_PARAM, "Gate");
		configSwitch(LATCH_PARAM, 0.0f, 1.0f, 0.0f, "Latch", {"Disabled", "Enabled"});
		configSwitch(LOGIC_MODE_PARAM, 0.0f, 4.0f, 0.0f, "Logic", {"OR", "AND", "XOR", "NOR", "NAND"});

		configInput(GATE_A_INPUT, "Gate A");
		configInput(GATE_B_INPUT, "Gate B");
		configInput(LOGIC_MODE_INPUT, "Logic CV");
		configInput(HIGH_INPUT, "High signal");
		configInput(LOW_INPUT, "Low signal");

		configOutput(OUT_OUTPUT, "Signal");
	}

	void resetChannel(int c);
	void reset() override;
	int channels() override;
	void channelsChanged(int before, int after) override;
	void modulate() override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int _c) override;
	void postProcessAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
