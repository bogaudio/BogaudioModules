#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMatrix88;

namespace bogaudio {

struct Matrix88 : Module {
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
		LEVEL_PARAM,
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

	enum LightsIds {
		NUM_LIGHTS
	};

	Saturator _saturators[8];

	Matrix88() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(MIX11_PARAM, -1.0f, 1.0f, 0.0f, "mix11");
		configParam(MIX21_PARAM, -1.0f, 1.0f, 0.0f, "mix21");
		configParam(MIX31_PARAM, -1.0f, 1.0f, 0.0f, "mix31");
		configParam(MIX41_PARAM, -1.0f, 1.0f, 0.0f, "mix41");
		configParam(MIX51_PARAM, -1.0f, 1.0f, 0.0f, "mix51");
		configParam(MIX61_PARAM, -1.0f, 1.0f, 0.0f, "mix61");
		configParam(MIX71_PARAM, -1.0f, 1.0f, 0.0f, "mix71");
		configParam(MIX81_PARAM, -1.0f, 1.0f, 0.0f, "mix81");
		configParam(MIX12_PARAM, -1.0f, 1.0f, 0.0f, "mix12");
		configParam(MIX22_PARAM, -1.0f, 1.0f, 0.0f, "mix22");
		configParam(MIX32_PARAM, -1.0f, 1.0f, 0.0f, "mix32");
		configParam(MIX42_PARAM, -1.0f, 1.0f, 0.0f, "mix42");
		configParam(MIX52_PARAM, -1.0f, 1.0f, 0.0f, "mix52");
		configParam(MIX62_PARAM, -1.0f, 1.0f, 0.0f, "mix62");
		configParam(MIX72_PARAM, -1.0f, 1.0f, 0.0f, "mix72");
		configParam(MIX82_PARAM, -1.0f, 1.0f, 0.0f, "mix82");
		configParam(MIX13_PARAM, -1.0f, 1.0f, 0.0f, "mix13");
		configParam(MIX23_PARAM, -1.0f, 1.0f, 0.0f, "mix23");
		configParam(MIX33_PARAM, -1.0f, 1.0f, 0.0f, "mix33");
		configParam(MIX43_PARAM, -1.0f, 1.0f, 0.0f, "mix43");
		configParam(MIX53_PARAM, -1.0f, 1.0f, 0.0f, "mix53");
		configParam(MIX63_PARAM, -1.0f, 1.0f, 0.0f, "mix63");
		configParam(MIX73_PARAM, -1.0f, 1.0f, 0.0f, "mix73");
		configParam(MIX83_PARAM, -1.0f, 1.0f, 0.0f, "mix83");
		configParam(MIX14_PARAM, -1.0f, 1.0f, 0.0f, "mix14");
		configParam(MIX24_PARAM, -1.0f, 1.0f, 0.0f, "mix24");
		configParam(MIX34_PARAM, -1.0f, 1.0f, 0.0f, "mix34");
		configParam(MIX44_PARAM, -1.0f, 1.0f, 0.0f, "mix44");
		configParam(MIX54_PARAM, -1.0f, 1.0f, 0.0f, "mix54");
		configParam(MIX64_PARAM, -1.0f, 1.0f, 0.0f, "mix64");
		configParam(MIX74_PARAM, -1.0f, 1.0f, 0.0f, "mix74");
		configParam(MIX84_PARAM, -1.0f, 1.0f, 0.0f, "mix84");
		configParam(MIX15_PARAM, -1.0f, 1.0f, 0.0f, "mix15");
		configParam(MIX25_PARAM, -1.0f, 1.0f, 0.0f, "mix25");
		configParam(MIX35_PARAM, -1.0f, 1.0f, 0.0f, "mix35");
		configParam(MIX45_PARAM, -1.0f, 1.0f, 0.0f, "mix45");
		configParam(MIX55_PARAM, -1.0f, 1.0f, 0.0f, "mix55");
		configParam(MIX65_PARAM, -1.0f, 1.0f, 0.0f, "mix65");
		configParam(MIX75_PARAM, -1.0f, 1.0f, 0.0f, "mix75");
		configParam(MIX85_PARAM, -1.0f, 1.0f, 0.0f, "mix85");
		configParam(MIX16_PARAM, -1.0f, 1.0f, 0.0f, "mix16");
		configParam(MIX26_PARAM, -1.0f, 1.0f, 0.0f, "mix26");
		configParam(MIX36_PARAM, -1.0f, 1.0f, 0.0f, "mix36");
		configParam(MIX46_PARAM, -1.0f, 1.0f, 0.0f, "mix46");
		configParam(MIX56_PARAM, -1.0f, 1.0f, 0.0f, "mix56");
		configParam(MIX66_PARAM, -1.0f, 1.0f, 0.0f, "mix66");
		configParam(MIX76_PARAM, -1.0f, 1.0f, 0.0f, "mix76");
		configParam(MIX86_PARAM, -1.0f, 1.0f, 0.0f, "mix86");
		configParam(MIX17_PARAM, -1.0f, 1.0f, 0.0f, "mix17");
		configParam(MIX27_PARAM, -1.0f, 1.0f, 0.0f, "mix27");
		configParam(MIX37_PARAM, -1.0f, 1.0f, 0.0f, "mix37");
		configParam(MIX47_PARAM, -1.0f, 1.0f, 0.0f, "mix47");
		configParam(MIX57_PARAM, -1.0f, 1.0f, 0.0f, "mix57");
		configParam(MIX67_PARAM, -1.0f, 1.0f, 0.0f, "mix67");
		configParam(MIX77_PARAM, -1.0f, 1.0f, 0.0f, "mix77");
		configParam(MIX87_PARAM, -1.0f, 1.0f, 0.0f, "mix87");
		configParam(MIX18_PARAM, -1.0f, 1.0f, 0.0f, "mix18");
		configParam(MIX28_PARAM, -1.0f, 1.0f, 0.0f, "mix28");
		configParam(MIX38_PARAM, -1.0f, 1.0f, 0.0f, "mix38");
		configParam(MIX48_PARAM, -1.0f, 1.0f, 0.0f, "mix48");
		configParam(MIX58_PARAM, -1.0f, 1.0f, 0.0f, "mix58");
		configParam(MIX68_PARAM, -1.0f, 1.0f, 0.0f, "mix68");
		configParam(MIX78_PARAM, -1.0f, 1.0f, 0.0f, "mix78");
		configParam(MIX88_PARAM, -1.0f, 1.0f, 0.0f, "mix88");
		configParam(LEVEL_PARAM, 0.0f, 1.0f, 1.0f, "level");
	}

	void process(const ProcessArgs& args) override;
};

} // namespace bogaudio
