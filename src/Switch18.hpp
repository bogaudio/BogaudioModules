#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"

extern Model* modelSwitch18;

namespace bogaudio {

struct Switch18 : SwitchMatrixModule {
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
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		OUT7_OUTPUT,
		OUT8_OUTPUT,
		NUM_OUTPUTS
	};

	Switch18() : SwitchMatrixModule(1, 8, MIX1_PARAM, IN_INPUT, OUT1_OUTPUT) {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configSwitchParam(MIX1_PARAM, "Route 1");
		configSwitchParam(MIX2_PARAM, "Route 2");
		configSwitchParam(MIX3_PARAM, "Route 3");
		configSwitchParam(MIX4_PARAM, "Route 4");
		configSwitchParam(MIX5_PARAM, "Route 5");
		configSwitchParam(MIX6_PARAM, "Route 6");
		configSwitchParam(MIX7_PARAM, "Route 7");
		configSwitchParam(MIX8_PARAM, "Route 8");

		configInput(IN_INPUT, "IN");

		configOutput(OUT1_OUTPUT, "OUT1");
		configOutput(OUT2_OUTPUT, "OUT2");
		configOutput(OUT3_OUTPUT, "OUT3");
		configOutput(OUT4_OUTPUT, "OUT4");
		configOutput(OUT5_OUTPUT, "OUT5");
		configOutput(OUT6_OUTPUT, "OUT6");
		configOutput(OUT7_OUTPUT, "OUT7");
		configOutput(OUT8_OUTPUT, "OUT8");
	}
};

} // namespace bogaudio
