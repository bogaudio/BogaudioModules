#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"

extern Model* modelSwitch1616;

namespace bogaudio {

struct Switch1616 : SwitchMatrixModule {
	enum ParamsIds {
		MIX_1_1_PARAM,
		MIX_1_2_PARAM,
		MIX_1_3_PARAM,
		MIX_1_4_PARAM,
		MIX_1_5_PARAM,
		MIX_1_6_PARAM,
		MIX_1_7_PARAM,
		MIX_1_8_PARAM,
		MIX_1_9_PARAM,
		MIX_1_10_PARAM,
		MIX_1_11_PARAM,
		MIX_1_12_PARAM,
		MIX_1_13_PARAM,
		MIX_1_14_PARAM,
		MIX_1_15_PARAM,
		MIX_1_16_PARAM,
		MIX_2_1_PARAM,
		MIX_2_2_PARAM,
		MIX_2_3_PARAM,
		MIX_2_4_PARAM,
		MIX_2_5_PARAM,
		MIX_2_6_PARAM,
		MIX_2_7_PARAM,
		MIX_2_8_PARAM,
		MIX_2_9_PARAM,
		MIX_2_10_PARAM,
		MIX_2_11_PARAM,
		MIX_2_12_PARAM,
		MIX_2_13_PARAM,
		MIX_2_14_PARAM,
		MIX_2_15_PARAM,
		MIX_2_16_PARAM,
		MIX_3_1_PARAM,
		MIX_3_2_PARAM,
		MIX_3_3_PARAM,
		MIX_3_4_PARAM,
		MIX_3_5_PARAM,
		MIX_3_6_PARAM,
		MIX_3_7_PARAM,
		MIX_3_8_PARAM,
		MIX_3_9_PARAM,
		MIX_3_10_PARAM,
		MIX_3_11_PARAM,
		MIX_3_12_PARAM,
		MIX_3_13_PARAM,
		MIX_3_14_PARAM,
		MIX_3_15_PARAM,
		MIX_3_16_PARAM,
		MIX_4_1_PARAM,
		MIX_4_2_PARAM,
		MIX_4_3_PARAM,
		MIX_4_4_PARAM,
		MIX_4_5_PARAM,
		MIX_4_6_PARAM,
		MIX_4_7_PARAM,
		MIX_4_8_PARAM,
		MIX_4_9_PARAM,
		MIX_4_10_PARAM,
		MIX_4_11_PARAM,
		MIX_4_12_PARAM,
		MIX_4_13_PARAM,
		MIX_4_14_PARAM,
		MIX_4_15_PARAM,
		MIX_4_16_PARAM,
		MIX_5_1_PARAM,
		MIX_5_2_PARAM,
		MIX_5_3_PARAM,
		MIX_5_4_PARAM,
		MIX_5_5_PARAM,
		MIX_5_6_PARAM,
		MIX_5_7_PARAM,
		MIX_5_8_PARAM,
		MIX_5_9_PARAM,
		MIX_5_10_PARAM,
		MIX_5_11_PARAM,
		MIX_5_12_PARAM,
		MIX_5_13_PARAM,
		MIX_5_14_PARAM,
		MIX_5_15_PARAM,
		MIX_5_16_PARAM,
		MIX_6_1_PARAM,
		MIX_6_2_PARAM,
		MIX_6_3_PARAM,
		MIX_6_4_PARAM,
		MIX_6_5_PARAM,
		MIX_6_6_PARAM,
		MIX_6_7_PARAM,
		MIX_6_8_PARAM,
		MIX_6_9_PARAM,
		MIX_6_10_PARAM,
		MIX_6_11_PARAM,
		MIX_6_12_PARAM,
		MIX_6_13_PARAM,
		MIX_6_14_PARAM,
		MIX_6_15_PARAM,
		MIX_6_16_PARAM,
		MIX_7_1_PARAM,
		MIX_7_2_PARAM,
		MIX_7_3_PARAM,
		MIX_7_4_PARAM,
		MIX_7_5_PARAM,
		MIX_7_6_PARAM,
		MIX_7_7_PARAM,
		MIX_7_8_PARAM,
		MIX_7_9_PARAM,
		MIX_7_10_PARAM,
		MIX_7_11_PARAM,
		MIX_7_12_PARAM,
		MIX_7_13_PARAM,
		MIX_7_14_PARAM,
		MIX_7_15_PARAM,
		MIX_7_16_PARAM,
		MIX_8_1_PARAM,
		MIX_8_2_PARAM,
		MIX_8_3_PARAM,
		MIX_8_4_PARAM,
		MIX_8_5_PARAM,
		MIX_8_6_PARAM,
		MIX_8_7_PARAM,
		MIX_8_8_PARAM,
		MIX_8_9_PARAM,
		MIX_8_10_PARAM,
		MIX_8_11_PARAM,
		MIX_8_12_PARAM,
		MIX_8_13_PARAM,
		MIX_8_14_PARAM,
		MIX_8_15_PARAM,
		MIX_8_16_PARAM,
		MIX_9_1_PARAM,
		MIX_9_2_PARAM,
		MIX_9_3_PARAM,
		MIX_9_4_PARAM,
		MIX_9_5_PARAM,
		MIX_9_6_PARAM,
		MIX_9_7_PARAM,
		MIX_9_8_PARAM,
		MIX_9_9_PARAM,
		MIX_9_10_PARAM,
		MIX_9_11_PARAM,
		MIX_9_12_PARAM,
		MIX_9_13_PARAM,
		MIX_9_14_PARAM,
		MIX_9_15_PARAM,
		MIX_9_16_PARAM,
		MIX_10_1_PARAM,
		MIX_10_2_PARAM,
		MIX_10_3_PARAM,
		MIX_10_4_PARAM,
		MIX_10_5_PARAM,
		MIX_10_6_PARAM,
		MIX_10_7_PARAM,
		MIX_10_8_PARAM,
		MIX_10_9_PARAM,
		MIX_10_10_PARAM,
		MIX_10_11_PARAM,
		MIX_10_12_PARAM,
		MIX_10_13_PARAM,
		MIX_10_14_PARAM,
		MIX_10_15_PARAM,
		MIX_10_16_PARAM,
		MIX_11_1_PARAM,
		MIX_11_2_PARAM,
		MIX_11_3_PARAM,
		MIX_11_4_PARAM,
		MIX_11_5_PARAM,
		MIX_11_6_PARAM,
		MIX_11_7_PARAM,
		MIX_11_8_PARAM,
		MIX_11_9_PARAM,
		MIX_11_10_PARAM,
		MIX_11_11_PARAM,
		MIX_11_12_PARAM,
		MIX_11_13_PARAM,
		MIX_11_14_PARAM,
		MIX_11_15_PARAM,
		MIX_11_16_PARAM,
		MIX_12_1_PARAM,
		MIX_12_2_PARAM,
		MIX_12_3_PARAM,
		MIX_12_4_PARAM,
		MIX_12_5_PARAM,
		MIX_12_6_PARAM,
		MIX_12_7_PARAM,
		MIX_12_8_PARAM,
		MIX_12_9_PARAM,
		MIX_12_10_PARAM,
		MIX_12_11_PARAM,
		MIX_12_12_PARAM,
		MIX_12_13_PARAM,
		MIX_12_14_PARAM,
		MIX_12_15_PARAM,
		MIX_12_16_PARAM,
		MIX_13_1_PARAM,
		MIX_13_2_PARAM,
		MIX_13_3_PARAM,
		MIX_13_4_PARAM,
		MIX_13_5_PARAM,
		MIX_13_6_PARAM,
		MIX_13_7_PARAM,
		MIX_13_8_PARAM,
		MIX_13_9_PARAM,
		MIX_13_10_PARAM,
		MIX_13_11_PARAM,
		MIX_13_12_PARAM,
		MIX_13_13_PARAM,
		MIX_13_14_PARAM,
		MIX_13_15_PARAM,
		MIX_13_16_PARAM,
		MIX_14_1_PARAM,
		MIX_14_2_PARAM,
		MIX_14_3_PARAM,
		MIX_14_4_PARAM,
		MIX_14_5_PARAM,
		MIX_14_6_PARAM,
		MIX_14_7_PARAM,
		MIX_14_8_PARAM,
		MIX_14_9_PARAM,
		MIX_14_10_PARAM,
		MIX_14_11_PARAM,
		MIX_14_12_PARAM,
		MIX_14_13_PARAM,
		MIX_14_14_PARAM,
		MIX_14_15_PARAM,
		MIX_14_16_PARAM,
		MIX_15_1_PARAM,
		MIX_15_2_PARAM,
		MIX_15_3_PARAM,
		MIX_15_4_PARAM,
		MIX_15_5_PARAM,
		MIX_15_6_PARAM,
		MIX_15_7_PARAM,
		MIX_15_8_PARAM,
		MIX_15_9_PARAM,
		MIX_15_10_PARAM,
		MIX_15_11_PARAM,
		MIX_15_12_PARAM,
		MIX_15_13_PARAM,
		MIX_15_14_PARAM,
		MIX_15_15_PARAM,
		MIX_15_16_PARAM,
		MIX_16_1_PARAM,
		MIX_16_2_PARAM,
		MIX_16_3_PARAM,
		MIX_16_4_PARAM,
		MIX_16_5_PARAM,
		MIX_16_6_PARAM,
		MIX_16_7_PARAM,
		MIX_16_8_PARAM,
		MIX_16_9_PARAM,
		MIX_16_10_PARAM,
		MIX_16_11_PARAM,
		MIX_16_12_PARAM,
		MIX_16_13_PARAM,
		MIX_16_14_PARAM,
		MIX_16_15_PARAM,
		MIX_16_16_PARAM,
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
		IN9_INPUT,
		IN10_INPUT,
		IN11_INPUT,
		IN12_INPUT,
		IN13_INPUT,
		IN14_INPUT,
		IN15_INPUT,
		IN16_INPUT,
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
		OUT9_OUTPUT,
		OUT10_OUTPUT,
		OUT11_OUTPUT,
		OUT12_OUTPUT,
		OUT13_OUTPUT,
		OUT14_OUTPUT,
		OUT15_OUTPUT,
		OUT16_OUTPUT,
		NUM_OUTPUTS
	};

	Switch1616() : SwitchMatrixModule(16, 16, MIX_1_1_PARAM, IN1_INPUT, OUT1_OUTPUT) {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

		configSwitchParam(MIX_1_1_PARAM, "Mix 1A");
		configSwitchParam(MIX_1_2_PARAM, "Mix 2A");
		configSwitchParam(MIX_1_3_PARAM, "Mix 3A");
		configSwitchParam(MIX_1_4_PARAM, "Mix 4A");
		configSwitchParam(MIX_1_5_PARAM, "Mix 5A");
		configSwitchParam(MIX_1_6_PARAM, "Mix 6A");
		configSwitchParam(MIX_1_7_PARAM, "Mix 7A");
		configSwitchParam(MIX_1_8_PARAM, "Mix 8A");
		configSwitchParam(MIX_1_9_PARAM, "Mix 9A");
		configSwitchParam(MIX_1_10_PARAM, "Mix 10A");
		configSwitchParam(MIX_1_11_PARAM, "Mix 11A");
		configSwitchParam(MIX_1_12_PARAM, "Mix 12A");
		configSwitchParam(MIX_1_13_PARAM, "Mix 13A");
		configSwitchParam(MIX_1_14_PARAM, "Mix 14A");
		configSwitchParam(MIX_1_15_PARAM, "Mix 15A");
		configSwitchParam(MIX_1_16_PARAM, "Mix 16A");

		configSwitchParam(MIX_2_1_PARAM, "Mix 1B");
		configSwitchParam(MIX_2_2_PARAM, "Mix 2B");
		configSwitchParam(MIX_2_3_PARAM, "Mix 3B");
		configSwitchParam(MIX_2_4_PARAM, "Mix 4B");
		configSwitchParam(MIX_2_5_PARAM, "Mix 5B");
		configSwitchParam(MIX_2_6_PARAM, "Mix 6B");
		configSwitchParam(MIX_2_7_PARAM, "Mix 7B");
		configSwitchParam(MIX_2_8_PARAM, "Mix 8B");
		configSwitchParam(MIX_2_9_PARAM, "Mix 9B");
		configSwitchParam(MIX_2_10_PARAM, "Mix 10B");
		configSwitchParam(MIX_2_11_PARAM, "Mix 11B");
		configSwitchParam(MIX_2_12_PARAM, "Mix 12B");
		configSwitchParam(MIX_2_13_PARAM, "Mix 13B");
		configSwitchParam(MIX_2_14_PARAM, "Mix 14B");
		configSwitchParam(MIX_2_15_PARAM, "Mix 15B");
		configSwitchParam(MIX_2_16_PARAM, "Mix 16B");

		configSwitchParam(MIX_3_1_PARAM, "Mix 1C");
		configSwitchParam(MIX_3_2_PARAM, "Mix 2C");
		configSwitchParam(MIX_3_3_PARAM, "Mix 3C");
		configSwitchParam(MIX_3_4_PARAM, "Mix 4C");
		configSwitchParam(MIX_3_5_PARAM, "Mix 5C");
		configSwitchParam(MIX_3_6_PARAM, "Mix 6C");
		configSwitchParam(MIX_3_7_PARAM, "Mix 7C");
		configSwitchParam(MIX_3_8_PARAM, "Mix 8C");
		configSwitchParam(MIX_3_9_PARAM, "Mix 9C");
		configSwitchParam(MIX_3_10_PARAM, "Mix 10C");
		configSwitchParam(MIX_3_11_PARAM, "Mix 11C");
		configSwitchParam(MIX_3_12_PARAM, "Mix 12C");
		configSwitchParam(MIX_3_13_PARAM, "Mix 13C");
		configSwitchParam(MIX_3_14_PARAM, "Mix 14C");
		configSwitchParam(MIX_3_15_PARAM, "Mix 15C");
		configSwitchParam(MIX_3_16_PARAM, "Mix 16C");

		configSwitchParam(MIX_4_1_PARAM, "Mix 1D");
		configSwitchParam(MIX_4_2_PARAM, "Mix 2D");
		configSwitchParam(MIX_4_3_PARAM, "Mix 3D");
		configSwitchParam(MIX_4_4_PARAM, "Mix 4D");
		configSwitchParam(MIX_4_5_PARAM, "Mix 5D");
		configSwitchParam(MIX_4_6_PARAM, "Mix 6D");
		configSwitchParam(MIX_4_7_PARAM, "Mix 7D");
		configSwitchParam(MIX_4_8_PARAM, "Mix 8D");
		configSwitchParam(MIX_4_9_PARAM, "Mix 9D");
		configSwitchParam(MIX_4_10_PARAM, "Mix 10D");
		configSwitchParam(MIX_4_11_PARAM, "Mix 11D");
		configSwitchParam(MIX_4_12_PARAM, "Mix 12D");
		configSwitchParam(MIX_4_13_PARAM, "Mix 13D");
		configSwitchParam(MIX_4_14_PARAM, "Mix 14D");
		configSwitchParam(MIX_4_15_PARAM, "Mix 15D");
		configSwitchParam(MIX_4_16_PARAM, "Mix 16D");

		configSwitchParam(MIX_5_1_PARAM, "Mix 1E");
		configSwitchParam(MIX_5_2_PARAM, "Mix 2E");
		configSwitchParam(MIX_5_3_PARAM, "Mix 3E");
		configSwitchParam(MIX_5_4_PARAM, "Mix 4E");
		configSwitchParam(MIX_5_5_PARAM, "Mix 5E");
		configSwitchParam(MIX_5_6_PARAM, "Mix 6E");
		configSwitchParam(MIX_5_7_PARAM, "Mix 7E");
		configSwitchParam(MIX_5_8_PARAM, "Mix 8E");
		configSwitchParam(MIX_5_9_PARAM, "Mix 9E");
		configSwitchParam(MIX_5_10_PARAM, "Mix 10E");
		configSwitchParam(MIX_5_11_PARAM, "Mix 11E");
		configSwitchParam(MIX_5_12_PARAM, "Mix 12E");
		configSwitchParam(MIX_5_13_PARAM, "Mix 13E");
		configSwitchParam(MIX_5_14_PARAM, "Mix 14E");
		configSwitchParam(MIX_5_15_PARAM, "Mix 15E");
		configSwitchParam(MIX_5_16_PARAM, "Mix 16E");

		configSwitchParam(MIX_6_1_PARAM, "Mix 1F");
		configSwitchParam(MIX_6_2_PARAM, "Mix 2F");
		configSwitchParam(MIX_6_3_PARAM, "Mix 3F");
		configSwitchParam(MIX_6_4_PARAM, "Mix 4F");
		configSwitchParam(MIX_6_5_PARAM, "Mix 5F");
		configSwitchParam(MIX_6_6_PARAM, "Mix 6F");
		configSwitchParam(MIX_6_7_PARAM, "Mix 7F");
		configSwitchParam(MIX_6_8_PARAM, "Mix 8F");
		configSwitchParam(MIX_6_9_PARAM, "Mix 9F");
		configSwitchParam(MIX_6_10_PARAM, "Mix 10F");
		configSwitchParam(MIX_6_11_PARAM, "Mix 11F");
		configSwitchParam(MIX_6_12_PARAM, "Mix 12F");
		configSwitchParam(MIX_6_13_PARAM, "Mix 13F");
		configSwitchParam(MIX_6_14_PARAM, "Mix 14F");
		configSwitchParam(MIX_6_15_PARAM, "Mix 15F");
		configSwitchParam(MIX_6_16_PARAM, "Mix 16F");

		configSwitchParam(MIX_7_1_PARAM, "Mix 1G");
		configSwitchParam(MIX_7_2_PARAM, "Mix 2G");
		configSwitchParam(MIX_7_3_PARAM, "Mix 3G");
		configSwitchParam(MIX_7_4_PARAM, "Mix 4G");
		configSwitchParam(MIX_7_5_PARAM, "Mix 5G");
		configSwitchParam(MIX_7_6_PARAM, "Mix 6G");
		configSwitchParam(MIX_7_7_PARAM, "Mix 7G");
		configSwitchParam(MIX_7_8_PARAM, "Mix 8G");
		configSwitchParam(MIX_7_9_PARAM, "Mix 9G");
		configSwitchParam(MIX_7_10_PARAM, "Mix 10G");
		configSwitchParam(MIX_7_11_PARAM, "Mix 11G");
		configSwitchParam(MIX_7_12_PARAM, "Mix 12G");
		configSwitchParam(MIX_7_13_PARAM, "Mix 13G");
		configSwitchParam(MIX_7_14_PARAM, "Mix 14G");
		configSwitchParam(MIX_7_15_PARAM, "Mix 15G");
		configSwitchParam(MIX_7_16_PARAM, "Mix 16G");

		configSwitchParam(MIX_8_1_PARAM, "Mix 1H");
		configSwitchParam(MIX_8_2_PARAM, "Mix 2H");
		configSwitchParam(MIX_8_3_PARAM, "Mix 3H");
		configSwitchParam(MIX_8_4_PARAM, "Mix 4H");
		configSwitchParam(MIX_8_5_PARAM, "Mix 5H");
		configSwitchParam(MIX_8_6_PARAM, "Mix 6H");
		configSwitchParam(MIX_8_7_PARAM, "Mix 7H");
		configSwitchParam(MIX_8_8_PARAM, "Mix 8H");
		configSwitchParam(MIX_8_9_PARAM, "Mix 9H");
		configSwitchParam(MIX_8_10_PARAM, "Mix 10H");
		configSwitchParam(MIX_8_11_PARAM, "Mix 11H");
		configSwitchParam(MIX_8_12_PARAM, "Mix 12H");
		configSwitchParam(MIX_8_13_PARAM, "Mix 13H");
		configSwitchParam(MIX_8_14_PARAM, "Mix 14H");
		configSwitchParam(MIX_8_15_PARAM, "Mix 15H");
		configSwitchParam(MIX_8_16_PARAM, "Mix 16H");

		configSwitchParam(MIX_9_1_PARAM, "Mix 1I");
		configSwitchParam(MIX_9_2_PARAM, "Mix 2I");
		configSwitchParam(MIX_9_3_PARAM, "Mix 3I");
		configSwitchParam(MIX_9_4_PARAM, "Mix 4I");
		configSwitchParam(MIX_9_5_PARAM, "Mix 5I");
		configSwitchParam(MIX_9_6_PARAM, "Mix 6I");
		configSwitchParam(MIX_9_7_PARAM, "Mix 7I");
		configSwitchParam(MIX_9_8_PARAM, "Mix 8I");
		configSwitchParam(MIX_9_9_PARAM, "Mix 9I");
		configSwitchParam(MIX_9_10_PARAM, "Mix 10I");
		configSwitchParam(MIX_9_11_PARAM, "Mix 11I");
		configSwitchParam(MIX_9_12_PARAM, "Mix 12I");
		configSwitchParam(MIX_9_13_PARAM, "Mix 13I");
		configSwitchParam(MIX_9_14_PARAM, "Mix 14I");
		configSwitchParam(MIX_9_15_PARAM, "Mix 15I");
		configSwitchParam(MIX_9_16_PARAM, "Mix 16I");

		configSwitchParam(MIX_10_1_PARAM, "Mix 1J");
		configSwitchParam(MIX_10_2_PARAM, "Mix 2J");
		configSwitchParam(MIX_10_3_PARAM, "Mix 3J");
		configSwitchParam(MIX_10_4_PARAM, "Mix 4J");
		configSwitchParam(MIX_10_5_PARAM, "Mix 5J");
		configSwitchParam(MIX_10_6_PARAM, "Mix 6J");
		configSwitchParam(MIX_10_7_PARAM, "Mix 7J");
		configSwitchParam(MIX_10_8_PARAM, "Mix 8J");
		configSwitchParam(MIX_10_9_PARAM, "Mix 9J");
		configSwitchParam(MIX_10_10_PARAM, "Mix 10J");
		configSwitchParam(MIX_10_11_PARAM, "Mix 11J");
		configSwitchParam(MIX_10_12_PARAM, "Mix 12J");
		configSwitchParam(MIX_10_13_PARAM, "Mix 13J");
		configSwitchParam(MIX_10_14_PARAM, "Mix 14J");
		configSwitchParam(MIX_10_15_PARAM, "Mix 15J");
		configSwitchParam(MIX_10_16_PARAM, "Mix 16J");

		configSwitchParam(MIX_11_1_PARAM, "Mix 1K");
		configSwitchParam(MIX_11_2_PARAM, "Mix 2K");
		configSwitchParam(MIX_11_3_PARAM, "Mix 3K");
		configSwitchParam(MIX_11_4_PARAM, "Mix 4K");
		configSwitchParam(MIX_11_5_PARAM, "Mix 5K");
		configSwitchParam(MIX_11_6_PARAM, "Mix 6K");
		configSwitchParam(MIX_11_7_PARAM, "Mix 7K");
		configSwitchParam(MIX_11_8_PARAM, "Mix 8K");
		configSwitchParam(MIX_11_9_PARAM, "Mix 9K");
		configSwitchParam(MIX_11_10_PARAM, "Mix 10K");
		configSwitchParam(MIX_11_11_PARAM, "Mix 11K");
		configSwitchParam(MIX_11_12_PARAM, "Mix 12K");
		configSwitchParam(MIX_11_13_PARAM, "Mix 13K");
		configSwitchParam(MIX_11_14_PARAM, "Mix 14K");
		configSwitchParam(MIX_11_15_PARAM, "Mix 15K");
		configSwitchParam(MIX_11_16_PARAM, "Mix 16K");

		configSwitchParam(MIX_12_1_PARAM, "Mix 1L");
		configSwitchParam(MIX_12_2_PARAM, "Mix 2L");
		configSwitchParam(MIX_12_3_PARAM, "Mix 3L");
		configSwitchParam(MIX_12_4_PARAM, "Mix 4L");
		configSwitchParam(MIX_12_5_PARAM, "Mix 5L");
		configSwitchParam(MIX_12_6_PARAM, "Mix 6L");
		configSwitchParam(MIX_12_7_PARAM, "Mix 7L");
		configSwitchParam(MIX_12_8_PARAM, "Mix 8L");
		configSwitchParam(MIX_12_9_PARAM, "Mix 9L");
		configSwitchParam(MIX_12_10_PARAM, "Mix 10L");
		configSwitchParam(MIX_12_11_PARAM, "Mix 11L");
		configSwitchParam(MIX_12_12_PARAM, "Mix 12L");
		configSwitchParam(MIX_12_13_PARAM, "Mix 13L");
		configSwitchParam(MIX_12_14_PARAM, "Mix 14L");
		configSwitchParam(MIX_12_15_PARAM, "Mix 15L");
		configSwitchParam(MIX_12_16_PARAM, "Mix 16L");

		configSwitchParam(MIX_13_1_PARAM, "Mix 1M");
		configSwitchParam(MIX_13_2_PARAM, "Mix 2M");
		configSwitchParam(MIX_13_3_PARAM, "Mix 3M");
		configSwitchParam(MIX_13_4_PARAM, "Mix 4M");
		configSwitchParam(MIX_13_5_PARAM, "Mix 5M");
		configSwitchParam(MIX_13_6_PARAM, "Mix 6M");
		configSwitchParam(MIX_13_7_PARAM, "Mix 7M");
		configSwitchParam(MIX_13_8_PARAM, "Mix 8M");
		configSwitchParam(MIX_13_9_PARAM, "Mix 9M");
		configSwitchParam(MIX_13_10_PARAM, "Mix 10M");
		configSwitchParam(MIX_13_11_PARAM, "Mix 11M");
		configSwitchParam(MIX_13_12_PARAM, "Mix 12M");
		configSwitchParam(MIX_13_13_PARAM, "Mix 13M");
		configSwitchParam(MIX_13_14_PARAM, "Mix 14M");
		configSwitchParam(MIX_13_15_PARAM, "Mix 15M");
		configSwitchParam(MIX_13_16_PARAM, "Mix 16M");

		configSwitchParam(MIX_14_1_PARAM, "Mix 1N");
		configSwitchParam(MIX_14_2_PARAM, "Mix 2N");
		configSwitchParam(MIX_14_3_PARAM, "Mix 3N");
		configSwitchParam(MIX_14_4_PARAM, "Mix 4N");
		configSwitchParam(MIX_14_5_PARAM, "Mix 5N");
		configSwitchParam(MIX_14_6_PARAM, "Mix 6N");
		configSwitchParam(MIX_14_7_PARAM, "Mix 7N");
		configSwitchParam(MIX_14_8_PARAM, "Mix 8N");
		configSwitchParam(MIX_14_9_PARAM, "Mix 9N");
		configSwitchParam(MIX_14_10_PARAM, "Mix 10N");
		configSwitchParam(MIX_14_11_PARAM, "Mix 11N");
		configSwitchParam(MIX_14_12_PARAM, "Mix 12N");
		configSwitchParam(MIX_14_13_PARAM, "Mix 13N");
		configSwitchParam(MIX_14_14_PARAM, "Mix 14N");
		configSwitchParam(MIX_14_15_PARAM, "Mix 15N");
		configSwitchParam(MIX_14_16_PARAM, "Mix 16N");

		configSwitchParam(MIX_15_1_PARAM, "Mix 1O");
		configSwitchParam(MIX_15_2_PARAM, "Mix 2O");
		configSwitchParam(MIX_15_3_PARAM, "Mix 3O");
		configSwitchParam(MIX_15_4_PARAM, "Mix 4O");
		configSwitchParam(MIX_15_5_PARAM, "Mix 5O");
		configSwitchParam(MIX_15_6_PARAM, "Mix 6O");
		configSwitchParam(MIX_15_7_PARAM, "Mix 7O");
		configSwitchParam(MIX_15_8_PARAM, "Mix 8O");
		configSwitchParam(MIX_15_9_PARAM, "Mix 9O");
		configSwitchParam(MIX_15_10_PARAM, "Mix 10O");
		configSwitchParam(MIX_15_11_PARAM, "Mix 11O");
		configSwitchParam(MIX_15_12_PARAM, "Mix 12O");
		configSwitchParam(MIX_15_13_PARAM, "Mix 13O");
		configSwitchParam(MIX_15_14_PARAM, "Mix 14O");
		configSwitchParam(MIX_15_15_PARAM, "Mix 15O");
		configSwitchParam(MIX_15_16_PARAM, "Mix 16O");

		configSwitchParam(MIX_16_1_PARAM, "Mix 1P");
		configSwitchParam(MIX_16_2_PARAM, "Mix 2P");
		configSwitchParam(MIX_16_3_PARAM, "Mix 3P");
		configSwitchParam(MIX_16_4_PARAM, "Mix 4P");
		configSwitchParam(MIX_16_5_PARAM, "Mix 5P");
		configSwitchParam(MIX_16_6_PARAM, "Mix 6P");
		configSwitchParam(MIX_16_7_PARAM, "Mix 7P");
		configSwitchParam(MIX_16_8_PARAM, "Mix 8P");
		configSwitchParam(MIX_16_9_PARAM, "Mix 9P");
		configSwitchParam(MIX_16_10_PARAM, "Mix 10P");
		configSwitchParam(MIX_16_11_PARAM, "Mix 11P");
		configSwitchParam(MIX_16_12_PARAM, "Mix 12P");
		configSwitchParam(MIX_16_13_PARAM, "Mix 13P");
		configSwitchParam(MIX_16_14_PARAM, "Mix 14P");
		configSwitchParam(MIX_16_15_PARAM, "Mix 15P");
		configSwitchParam(MIX_16_16_PARAM, "Mix 16P");

		configInput(IN1_INPUT, "Signal 1");
		configInput(IN2_INPUT, "Signal 2");
		configInput(IN3_INPUT, "Signal 3");
		configInput(IN4_INPUT, "Signal 4");
		configInput(IN5_INPUT, "Signal 5");
		configInput(IN6_INPUT, "Signal 6");
		configInput(IN7_INPUT, "Signal 7");
		configInput(IN8_INPUT, "Signal 8");
		configInput(IN9_INPUT, "Signal 9");
		configInput(IN10_INPUT, "Signal 10");
		configInput(IN11_INPUT, "Signal 11");
		configInput(IN12_INPUT, "Signal 12");
		configInput(IN13_INPUT, "Signal 13");
		configInput(IN14_INPUT, "Signal 14");
		configInput(IN15_INPUT, "Signal 15");
		configInput(IN16_INPUT, "Signal 16");

		configOutput(OUT1_OUTPUT, "Signal A");
		configOutput(OUT2_OUTPUT, "Signal B");
		configOutput(OUT3_OUTPUT, "Signal C");
		configOutput(OUT4_OUTPUT, "Signal D");
		configOutput(OUT5_OUTPUT, "Signal E");
		configOutput(OUT6_OUTPUT, "Signal F");
		configOutput(OUT7_OUTPUT, "Signal G");
		configOutput(OUT8_OUTPUT, "Signal H");
		configOutput(OUT9_OUTPUT, "Signal I");
		configOutput(OUT10_OUTPUT, "Signal J");
		configOutput(OUT11_OUTPUT, "Signal K");
		configOutput(OUT12_OUTPUT, "Signal L");
		configOutput(OUT13_OUTPUT, "Signal M");
		configOutput(OUT14_OUTPUT, "Signal N");
		configOutput(OUT15_OUTPUT, "Signal O");
		configOutput(OUT16_OUTPUT, "Signal P");
	}
};

} // namespace bogaudio
