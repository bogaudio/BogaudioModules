#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"

extern Model* modelSwitch44;

namespace bogaudio {

struct Switch44 : SwitchMatrixModule {
	enum ParamsIds {
		MIX11_PARAM,
		MIX21_PARAM,
		MIX31_PARAM,
		MIX41_PARAM,
		MIX12_PARAM,
		MIX22_PARAM,
		MIX32_PARAM,
		MIX42_PARAM,
		MIX13_PARAM,
		MIX23_PARAM,
		MIX33_PARAM,
		MIX43_PARAM,
		MIX14_PARAM,
		MIX24_PARAM,
		MIX34_PARAM,
		MIX44_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		NUM_OUTPUTS
	};

	Switch44() : SwitchMatrixModule(4, 4, MIX11_PARAM, IN1_INPUT, OUT1_OUTPUT) {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configSwitchParam(MIX11_PARAM, "Mix 1A");
		configSwitchParam(MIX21_PARAM, "Mix 2A");
		configSwitchParam(MIX31_PARAM, "Mix 3A");
		configSwitchParam(MIX41_PARAM, "Mix 4A");
		configSwitchParam(MIX12_PARAM, "Mix 1B");
		configSwitchParam(MIX22_PARAM, "Mix 2B");
		configSwitchParam(MIX32_PARAM, "Mix 3B");
		configSwitchParam(MIX42_PARAM, "Mix 4B");
		configSwitchParam(MIX13_PARAM, "Mix 1C");
		configSwitchParam(MIX23_PARAM, "Mix 2C");
		configSwitchParam(MIX33_PARAM, "Mix 3C");
		configSwitchParam(MIX43_PARAM, "Mix 4C");
		configSwitchParam(MIX14_PARAM, "Mix 1D");
		configSwitchParam(MIX24_PARAM, "Mix 2D");
		configSwitchParam(MIX34_PARAM, "Mix 3D");
		configSwitchParam(MIX44_PARAM, "Mix 4D");

		configInput(IN1_INPUT, "Signal 1");
		configInput(IN2_INPUT, "Signal 2");
		configInput(IN3_INPUT, "Signal 3");
		configInput(IN4_INPUT, "Signal 4");

		configOutput(OUT1_OUTPUT, "OUT1");
		configOutput(OUT2_OUTPUT, "OUT2");
		configOutput(OUT3_OUTPUT, "OUT3");
		configOutput(OUT4_OUTPUT, "OUT4");
	}
};

} // namespace bogaudio
