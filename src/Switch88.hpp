#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"

extern Model* modelSwitch88;

namespace bogaudio {

struct Switch88 : MatrixModule {
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

	Switch88() : MatrixModule(8, MIX11_PARAM, IN1_INPUT, OUT1_OUTPUT) {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(MIX11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1A", "%", 0.0f, 100.0f);
		configParam(MIX21_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2A", "%", 0.0f, 100.0f);
		configParam(MIX31_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3A", "%", 0.0f, 100.0f);
		configParam(MIX41_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4A", "%", 0.0f, 100.0f);
		configParam(MIX51_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5A", "%", 0.0f, 100.0f);
		configParam(MIX61_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6A", "%", 0.0f, 100.0f);
		configParam(MIX71_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7A", "%", 0.0f, 100.0f);
		configParam(MIX81_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8A", "%", 0.0f, 100.0f);
		configParam(MIX12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1B", "%", 0.0f, 100.0f);
		configParam(MIX22_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2B", "%", 0.0f, 100.0f);
		configParam(MIX32_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3B", "%", 0.0f, 100.0f);
		configParam(MIX42_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4B", "%", 0.0f, 100.0f);
		configParam(MIX52_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5B", "%", 0.0f, 100.0f);
		configParam(MIX62_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6B", "%", 0.0f, 100.0f);
		configParam(MIX72_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7B", "%", 0.0f, 100.0f);
		configParam(MIX82_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8B", "%", 0.0f, 100.0f);
		configParam(MIX13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1C", "%", 0.0f, 100.0f);
		configParam(MIX23_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2C", "%", 0.0f, 100.0f);
		configParam(MIX33_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3C", "%", 0.0f, 100.0f);
		configParam(MIX43_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4C", "%", 0.0f, 100.0f);
		configParam(MIX53_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5C", "%", 0.0f, 100.0f);
		configParam(MIX63_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6C", "%", 0.0f, 100.0f);
		configParam(MIX73_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7C", "%", 0.0f, 100.0f);
		configParam(MIX83_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8C", "%", 0.0f, 100.0f);
		configParam(MIX14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1D", "%", 0.0f, 100.0f);
		configParam(MIX24_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2D", "%", 0.0f, 100.0f);
		configParam(MIX34_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3D", "%", 0.0f, 100.0f);
		configParam(MIX44_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4D", "%", 0.0f, 100.0f);
		configParam(MIX54_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5D", "%", 0.0f, 100.0f);
		configParam(MIX64_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6D", "%", 0.0f, 100.0f);
		configParam(MIX74_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7D", "%", 0.0f, 100.0f);
		configParam(MIX84_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8D", "%", 0.0f, 100.0f);
		configParam(MIX15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1E", "%", 0.0f, 100.0f);
		configParam(MIX25_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2E", "%", 0.0f, 100.0f);
		configParam(MIX35_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3E", "%", 0.0f, 100.0f);
		configParam(MIX45_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4E", "%", 0.0f, 100.0f);
		configParam(MIX55_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5E", "%", 0.0f, 100.0f);
		configParam(MIX65_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6E", "%", 0.0f, 100.0f);
		configParam(MIX75_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7E", "%", 0.0f, 100.0f);
		configParam(MIX85_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8E", "%", 0.0f, 100.0f);
		configParam(MIX16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1F", "%", 0.0f, 100.0f);
		configParam(MIX26_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2F", "%", 0.0f, 100.0f);
		configParam(MIX36_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3F", "%", 0.0f, 100.0f);
		configParam(MIX46_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4F", "%", 0.0f, 100.0f);
		configParam(MIX56_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5F", "%", 0.0f, 100.0f);
		configParam(MIX66_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6F", "%", 0.0f, 100.0f);
		configParam(MIX76_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7F", "%", 0.0f, 100.0f);
		configParam(MIX86_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8F", "%", 0.0f, 100.0f);
		configParam(MIX17_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1G", "%", 0.0f, 100.0f);
		configParam(MIX27_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2G", "%", 0.0f, 100.0f);
		configParam(MIX37_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3G", "%", 0.0f, 100.0f);
		configParam(MIX47_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4G", "%", 0.0f, 100.0f);
		configParam(MIX57_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5G", "%", 0.0f, 100.0f);
		configParam(MIX67_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6G", "%", 0.0f, 100.0f);
		configParam(MIX77_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7G", "%", 0.0f, 100.0f);
		configParam(MIX87_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8G", "%", 0.0f, 100.0f);
		configParam(MIX18_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1H", "%", 0.0f, 100.0f);
		configParam(MIX28_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2H", "%", 0.0f, 100.0f);
		configParam(MIX38_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3H", "%", 0.0f, 100.0f);
		configParam(MIX48_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4H", "%", 0.0f, 100.0f);
		configParam(MIX58_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5H", "%", 0.0f, 100.0f);
		configParam(MIX68_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6H", "%", 0.0f, 100.0f);
		configParam(MIX78_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7H", "%", 0.0f, 100.0f);
		configParam(MIX88_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8H", "%", 0.0f, 100.0f);
	}
};

} // namespace bogaudio
