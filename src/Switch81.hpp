#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"

extern Model* modelSwitch81;

namespace bogaudio {

struct Switch81 : SwitchMatrixModule {
	enum ParamsIds {
		MIX1_PARAM,
		MIX2_PARAM,
		MIX3_PARAM,
		MIX4_PARAM,
		MIX5_PARAM,
		MIX6_PARAM,
		MIX7_PARAM,
		MIX8_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		IN5_INPUT,
		IN6_INPUT,
		IN7_INPUT,
		IN8_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	Switch81() : SwitchMatrixModule(8, 1, MIX1_PARAM, IN1_INPUT, OUT_OUTPUT) {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configSwitchParam(MIX1_PARAM, "Mix 1");
		configSwitchParam(MIX2_PARAM, "Mix 2");
		configSwitchParam(MIX3_PARAM, "Mix 3");
		configSwitchParam(MIX4_PARAM, "Mix 4");
		configSwitchParam(MIX5_PARAM, "Mix 5");
		configSwitchParam(MIX6_PARAM, "Mix 6");
		configSwitchParam(MIX7_PARAM, "Mix 7");
		configSwitchParam(MIX8_PARAM, "Mix 8");

		configInput(IN1_INPUT, "Signal 1");
		configInput(IN2_INPUT, "Signal 2");
		configInput(IN3_INPUT, "Signal 3");
		configInput(IN4_INPUT, "Signal 4");
		configInput(IN5_INPUT, "Signal 5");
		configInput(IN6_INPUT, "Signal 6");
		configInput(IN7_INPUT, "Signal 7");
		configInput(IN8_INPUT, "Signal 8");

		configOutput(OUT_OUTPUT, "OUT");
	}
};

} // namespace bogaudio
