#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"

extern Model* modelSwitch88;

namespace bogaudio {

struct Switch88 : SwitchMatrixModule {
	enum ParamsIds {
		MIX11_PARAM,
		MIX21_PARAM,
		MIX31_PARAM,
		MIX41_PARAM,
		MIX51_PARAM,
		MIX61_PARAM,
		MIX71_PARAM,
		MIX81_PARAM,
		MIX12_PARAM,
		MIX22_PARAM,
		MIX32_PARAM,
		MIX42_PARAM,
		MIX52_PARAM,
		MIX62_PARAM,
		MIX72_PARAM,
		MIX82_PARAM,
		MIX13_PARAM,
		MIX23_PARAM,
		MIX33_PARAM,
		MIX43_PARAM,
		MIX53_PARAM,
		MIX63_PARAM,
		MIX73_PARAM,
		MIX83_PARAM,
		MIX14_PARAM,
		MIX24_PARAM,
		MIX34_PARAM,
		MIX44_PARAM,
		MIX54_PARAM,
		MIX64_PARAM,
		MIX74_PARAM,
		MIX84_PARAM,
		MIX15_PARAM,
		MIX25_PARAM,
		MIX35_PARAM,
		MIX45_PARAM,
		MIX55_PARAM,
		MIX65_PARAM,
		MIX75_PARAM,
		MIX85_PARAM,
		MIX16_PARAM,
		MIX26_PARAM,
		MIX36_PARAM,
		MIX46_PARAM,
		MIX56_PARAM,
		MIX66_PARAM,
		MIX76_PARAM,
		MIX86_PARAM,
		MIX17_PARAM,
		MIX27_PARAM,
		MIX37_PARAM,
		MIX47_PARAM,
		MIX57_PARAM,
		MIX67_PARAM,
		MIX77_PARAM,
		MIX87_PARAM,
		MIX18_PARAM,
		MIX28_PARAM,
		MIX38_PARAM,
		MIX48_PARAM,
		MIX58_PARAM,
		MIX68_PARAM,
		MIX78_PARAM,
		MIX88_PARAM,
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

	Switch88() : SwitchMatrixModule(8, 8, MIX11_PARAM, IN1_INPUT, OUT1_OUTPUT) {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configSwitchParam(MIX11_PARAM, "Mix 1A");
		configSwitchParam(MIX21_PARAM, "Mix 2A");
		configSwitchParam(MIX31_PARAM, "Mix 3A");
		configSwitchParam(MIX41_PARAM, "Mix 4A");
		configSwitchParam(MIX51_PARAM, "Mix 5A");
		configSwitchParam(MIX61_PARAM, "Mix 6A");
		configSwitchParam(MIX71_PARAM, "Mix 7A");
		configSwitchParam(MIX81_PARAM, "Mix 8A");
		configSwitchParam(MIX12_PARAM, "Mix 1B");
		configSwitchParam(MIX22_PARAM, "Mix 2B");
		configSwitchParam(MIX32_PARAM, "Mix 3B");
		configSwitchParam(MIX42_PARAM, "Mix 4B");
		configSwitchParam(MIX52_PARAM, "Mix 5B");
		configSwitchParam(MIX62_PARAM, "Mix 6B");
		configSwitchParam(MIX72_PARAM, "Mix 7B");
		configSwitchParam(MIX82_PARAM, "Mix 8B");
		configSwitchParam(MIX13_PARAM, "Mix 1C");
		configSwitchParam(MIX23_PARAM, "Mix 2C");
		configSwitchParam(MIX33_PARAM, "Mix 3C");
		configSwitchParam(MIX43_PARAM, "Mix 4C");
		configSwitchParam(MIX53_PARAM, "Mix 5C");
		configSwitchParam(MIX63_PARAM, "Mix 6C");
		configSwitchParam(MIX73_PARAM, "Mix 7C");
		configSwitchParam(MIX83_PARAM, "Mix 8C");
		configSwitchParam(MIX14_PARAM, "Mix 1D");
		configSwitchParam(MIX24_PARAM, "Mix 2D");
		configSwitchParam(MIX34_PARAM, "Mix 3D");
		configSwitchParam(MIX44_PARAM, "Mix 4D");
		configSwitchParam(MIX54_PARAM, "Mix 5D");
		configSwitchParam(MIX64_PARAM, "Mix 6D");
		configSwitchParam(MIX74_PARAM, "Mix 7D");
		configSwitchParam(MIX84_PARAM, "Mix 8D");
		configSwitchParam(MIX15_PARAM, "Mix 1E");
		configSwitchParam(MIX25_PARAM, "Mix 2E");
		configSwitchParam(MIX35_PARAM, "Mix 3E");
		configSwitchParam(MIX45_PARAM, "Mix 4E");
		configSwitchParam(MIX55_PARAM, "Mix 5E");
		configSwitchParam(MIX65_PARAM, "Mix 6E");
		configSwitchParam(MIX75_PARAM, "Mix 7E");
		configSwitchParam(MIX85_PARAM, "Mix 8E");
		configSwitchParam(MIX16_PARAM, "Mix 1F");
		configSwitchParam(MIX26_PARAM, "Mix 2F");
		configSwitchParam(MIX36_PARAM, "Mix 3F");
		configSwitchParam(MIX46_PARAM, "Mix 4F");
		configSwitchParam(MIX56_PARAM, "Mix 5F");
		configSwitchParam(MIX66_PARAM, "Mix 6F");
		configSwitchParam(MIX76_PARAM, "Mix 7F");
		configSwitchParam(MIX86_PARAM, "Mix 8F");
		configSwitchParam(MIX17_PARAM, "Mix 1G");
		configSwitchParam(MIX27_PARAM, "Mix 2G");
		configSwitchParam(MIX37_PARAM, "Mix 3G");
		configSwitchParam(MIX47_PARAM, "Mix 4G");
		configSwitchParam(MIX57_PARAM, "Mix 5G");
		configSwitchParam(MIX67_PARAM, "Mix 6G");
		configSwitchParam(MIX77_PARAM, "Mix 7G");
		configSwitchParam(MIX87_PARAM, "Mix 8G");
		configSwitchParam(MIX18_PARAM, "Mix 1H");
		configSwitchParam(MIX28_PARAM, "Mix 2H");
		configSwitchParam(MIX38_PARAM, "Mix 3H");
		configSwitchParam(MIX48_PARAM, "Mix 4H");
		configSwitchParam(MIX58_PARAM, "Mix 5H");
		configSwitchParam(MIX68_PARAM, "Mix 6H");
		configSwitchParam(MIX78_PARAM, "Mix 7H");
		configSwitchParam(MIX88_PARAM, "Mix 8H");

		configInput(IN1_INPUT, "Signal 1");
		configInput(IN2_INPUT, "Signal 2");
		configInput(IN3_INPUT, "Signal 3");
		configInput(IN4_INPUT, "Signal 4");
		configInput(IN5_INPUT, "Signal 5");
		configInput(IN6_INPUT, "Signal 6");
		configInput(IN7_INPUT, "Signal 7");
		configInput(IN8_INPUT, "Signal 8");

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
