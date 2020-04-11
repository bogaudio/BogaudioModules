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

	Switch1616() : SwitchMatrixModule(16, MIX_1_1_PARAM, IN1_INPUT, OUT1_OUTPUT) {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

		configParam(MIX_1_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1A", "%", 0.0f, 100.0f);
		configParam(MIX_1_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2A", "%", 0.0f, 100.0f);
		configParam(MIX_1_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3A", "%", 0.0f, 100.0f);
		configParam(MIX_1_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4A", "%", 0.0f, 100.0f);
		configParam(MIX_1_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5A", "%", 0.0f, 100.0f);
		configParam(MIX_1_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6A", "%", 0.0f, 100.0f);
		configParam(MIX_1_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7A", "%", 0.0f, 100.0f);
		configParam(MIX_1_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8A", "%", 0.0f, 100.0f);
		configParam(MIX_1_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9A", "%", 0.0f, 100.0f);
		configParam(MIX_1_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10A", "%", 0.0f, 100.0f);
		configParam(MIX_1_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11A", "%", 0.0f, 100.0f);
		configParam(MIX_1_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12A", "%", 0.0f, 100.0f);
		configParam(MIX_1_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13A", "%", 0.0f, 100.0f);
		configParam(MIX_1_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14A", "%", 0.0f, 100.0f);
		configParam(MIX_1_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15A", "%", 0.0f, 100.0f);
		configParam(MIX_1_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16A", "%", 0.0f, 100.0f);

		configParam(MIX_2_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1B", "%", 0.0f, 100.0f);
		configParam(MIX_2_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2B", "%", 0.0f, 100.0f);
		configParam(MIX_2_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3B", "%", 0.0f, 100.0f);
		configParam(MIX_2_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4B", "%", 0.0f, 100.0f);
		configParam(MIX_2_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5B", "%", 0.0f, 100.0f);
		configParam(MIX_2_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6B", "%", 0.0f, 100.0f);
		configParam(MIX_2_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7B", "%", 0.0f, 100.0f);
		configParam(MIX_2_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8B", "%", 0.0f, 100.0f);
		configParam(MIX_2_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9B", "%", 0.0f, 100.0f);
		configParam(MIX_2_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10B", "%", 0.0f, 100.0f);
		configParam(MIX_2_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11B", "%", 0.0f, 100.0f);
		configParam(MIX_2_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12B", "%", 0.0f, 100.0f);
		configParam(MIX_2_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13B", "%", 0.0f, 100.0f);
		configParam(MIX_2_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14B", "%", 0.0f, 100.0f);
		configParam(MIX_2_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15B", "%", 0.0f, 100.0f);
		configParam(MIX_2_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16B", "%", 0.0f, 100.0f);

		configParam(MIX_3_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1C", "%", 0.0f, 100.0f);
		configParam(MIX_3_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2C", "%", 0.0f, 100.0f);
		configParam(MIX_3_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3C", "%", 0.0f, 100.0f);
		configParam(MIX_3_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4C", "%", 0.0f, 100.0f);
		configParam(MIX_3_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5C", "%", 0.0f, 100.0f);
		configParam(MIX_3_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6C", "%", 0.0f, 100.0f);
		configParam(MIX_3_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7C", "%", 0.0f, 100.0f);
		configParam(MIX_3_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8C", "%", 0.0f, 100.0f);
		configParam(MIX_3_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9C", "%", 0.0f, 100.0f);
		configParam(MIX_3_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10C", "%", 0.0f, 100.0f);
		configParam(MIX_3_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11C", "%", 0.0f, 100.0f);
		configParam(MIX_3_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12C", "%", 0.0f, 100.0f);
		configParam(MIX_3_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13C", "%", 0.0f, 100.0f);
		configParam(MIX_3_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14C", "%", 0.0f, 100.0f);
		configParam(MIX_3_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15C", "%", 0.0f, 100.0f);
		configParam(MIX_3_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16C", "%", 0.0f, 100.0f);

		configParam(MIX_4_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1D", "%", 0.0f, 100.0f);
		configParam(MIX_4_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2D", "%", 0.0f, 100.0f);
		configParam(MIX_4_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3D", "%", 0.0f, 100.0f);
		configParam(MIX_4_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4D", "%", 0.0f, 100.0f);
		configParam(MIX_4_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5D", "%", 0.0f, 100.0f);
		configParam(MIX_4_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6D", "%", 0.0f, 100.0f);
		configParam(MIX_4_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7D", "%", 0.0f, 100.0f);
		configParam(MIX_4_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8D", "%", 0.0f, 100.0f);
		configParam(MIX_4_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9D", "%", 0.0f, 100.0f);
		configParam(MIX_4_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10D", "%", 0.0f, 100.0f);
		configParam(MIX_4_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11D", "%", 0.0f, 100.0f);
		configParam(MIX_4_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12D", "%", 0.0f, 100.0f);
		configParam(MIX_4_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13D", "%", 0.0f, 100.0f);
		configParam(MIX_4_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14D", "%", 0.0f, 100.0f);
		configParam(MIX_4_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15D", "%", 0.0f, 100.0f);
		configParam(MIX_4_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16D", "%", 0.0f, 100.0f);

		configParam(MIX_5_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1E", "%", 0.0f, 100.0f);
		configParam(MIX_5_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2E", "%", 0.0f, 100.0f);
		configParam(MIX_5_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3E", "%", 0.0f, 100.0f);
		configParam(MIX_5_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4E", "%", 0.0f, 100.0f);
		configParam(MIX_5_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5E", "%", 0.0f, 100.0f);
		configParam(MIX_5_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6E", "%", 0.0f, 100.0f);
		configParam(MIX_5_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7E", "%", 0.0f, 100.0f);
		configParam(MIX_5_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8E", "%", 0.0f, 100.0f);
		configParam(MIX_5_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9E", "%", 0.0f, 100.0f);
		configParam(MIX_5_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10E", "%", 0.0f, 100.0f);
		configParam(MIX_5_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11E", "%", 0.0f, 100.0f);
		configParam(MIX_5_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12E", "%", 0.0f, 100.0f);
		configParam(MIX_5_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13E", "%", 0.0f, 100.0f);
		configParam(MIX_5_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14E", "%", 0.0f, 100.0f);
		configParam(MIX_5_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15E", "%", 0.0f, 100.0f);
		configParam(MIX_5_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16E", "%", 0.0f, 100.0f);

		configParam(MIX_6_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1F", "%", 0.0f, 100.0f);
		configParam(MIX_6_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2F", "%", 0.0f, 100.0f);
		configParam(MIX_6_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3F", "%", 0.0f, 100.0f);
		configParam(MIX_6_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4F", "%", 0.0f, 100.0f);
		configParam(MIX_6_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5F", "%", 0.0f, 100.0f);
		configParam(MIX_6_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6F", "%", 0.0f, 100.0f);
		configParam(MIX_6_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7F", "%", 0.0f, 100.0f);
		configParam(MIX_6_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8F", "%", 0.0f, 100.0f);
		configParam(MIX_6_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9F", "%", 0.0f, 100.0f);
		configParam(MIX_6_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10F", "%", 0.0f, 100.0f);
		configParam(MIX_6_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11F", "%", 0.0f, 100.0f);
		configParam(MIX_6_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12F", "%", 0.0f, 100.0f);
		configParam(MIX_6_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13F", "%", 0.0f, 100.0f);
		configParam(MIX_6_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14F", "%", 0.0f, 100.0f);
		configParam(MIX_6_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15F", "%", 0.0f, 100.0f);
		configParam(MIX_6_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16F", "%", 0.0f, 100.0f);

		configParam(MIX_7_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1G", "%", 0.0f, 100.0f);
		configParam(MIX_7_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2G", "%", 0.0f, 100.0f);
		configParam(MIX_7_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3G", "%", 0.0f, 100.0f);
		configParam(MIX_7_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4G", "%", 0.0f, 100.0f);
		configParam(MIX_7_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5G", "%", 0.0f, 100.0f);
		configParam(MIX_7_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6G", "%", 0.0f, 100.0f);
		configParam(MIX_7_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7G", "%", 0.0f, 100.0f);
		configParam(MIX_7_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8G", "%", 0.0f, 100.0f);
		configParam(MIX_7_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9G", "%", 0.0f, 100.0f);
		configParam(MIX_7_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10G", "%", 0.0f, 100.0f);
		configParam(MIX_7_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11G", "%", 0.0f, 100.0f);
		configParam(MIX_7_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12G", "%", 0.0f, 100.0f);
		configParam(MIX_7_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13G", "%", 0.0f, 100.0f);
		configParam(MIX_7_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14G", "%", 0.0f, 100.0f);
		configParam(MIX_7_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15G", "%", 0.0f, 100.0f);
		configParam(MIX_7_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16G", "%", 0.0f, 100.0f);

		configParam(MIX_8_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1H", "%", 0.0f, 100.0f);
		configParam(MIX_8_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2H", "%", 0.0f, 100.0f);
		configParam(MIX_8_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3H", "%", 0.0f, 100.0f);
		configParam(MIX_8_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4H", "%", 0.0f, 100.0f);
		configParam(MIX_8_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5H", "%", 0.0f, 100.0f);
		configParam(MIX_8_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6H", "%", 0.0f, 100.0f);
		configParam(MIX_8_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7H", "%", 0.0f, 100.0f);
		configParam(MIX_8_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8H", "%", 0.0f, 100.0f);
		configParam(MIX_8_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9H", "%", 0.0f, 100.0f);
		configParam(MIX_8_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10H", "%", 0.0f, 100.0f);
		configParam(MIX_8_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11H", "%", 0.0f, 100.0f);
		configParam(MIX_8_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12H", "%", 0.0f, 100.0f);
		configParam(MIX_8_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13H", "%", 0.0f, 100.0f);
		configParam(MIX_8_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14H", "%", 0.0f, 100.0f);
		configParam(MIX_8_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15H", "%", 0.0f, 100.0f);
		configParam(MIX_8_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16H", "%", 0.0f, 100.0f);

		configParam(MIX_9_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1I", "%", 0.0f, 100.0f);
		configParam(MIX_9_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2I", "%", 0.0f, 100.0f);
		configParam(MIX_9_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3I", "%", 0.0f, 100.0f);
		configParam(MIX_9_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4I", "%", 0.0f, 100.0f);
		configParam(MIX_9_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5I", "%", 0.0f, 100.0f);
		configParam(MIX_9_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6I", "%", 0.0f, 100.0f);
		configParam(MIX_9_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7I", "%", 0.0f, 100.0f);
		configParam(MIX_9_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8I", "%", 0.0f, 100.0f);
		configParam(MIX_9_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9I", "%", 0.0f, 100.0f);
		configParam(MIX_9_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10I", "%", 0.0f, 100.0f);
		configParam(MIX_9_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11I", "%", 0.0f, 100.0f);
		configParam(MIX_9_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12I", "%", 0.0f, 100.0f);
		configParam(MIX_9_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13I", "%", 0.0f, 100.0f);
		configParam(MIX_9_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14I", "%", 0.0f, 100.0f);
		configParam(MIX_9_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15I", "%", 0.0f, 100.0f);
		configParam(MIX_9_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16I", "%", 0.0f, 100.0f);

		configParam(MIX_10_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1J", "%", 0.0f, 100.0f);
		configParam(MIX_10_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2J", "%", 0.0f, 100.0f);
		configParam(MIX_10_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3J", "%", 0.0f, 100.0f);
		configParam(MIX_10_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4J", "%", 0.0f, 100.0f);
		configParam(MIX_10_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5J", "%", 0.0f, 100.0f);
		configParam(MIX_10_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6J", "%", 0.0f, 100.0f);
		configParam(MIX_10_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7J", "%", 0.0f, 100.0f);
		configParam(MIX_10_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8J", "%", 0.0f, 100.0f);
		configParam(MIX_10_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9J", "%", 0.0f, 100.0f);
		configParam(MIX_10_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10J", "%", 0.0f, 100.0f);
		configParam(MIX_10_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11J", "%", 0.0f, 100.0f);
		configParam(MIX_10_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12J", "%", 0.0f, 100.0f);
		configParam(MIX_10_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13J", "%", 0.0f, 100.0f);
		configParam(MIX_10_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14J", "%", 0.0f, 100.0f);
		configParam(MIX_10_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15J", "%", 0.0f, 100.0f);
		configParam(MIX_10_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16J", "%", 0.0f, 100.0f);

		configParam(MIX_11_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1K", "%", 0.0f, 100.0f);
		configParam(MIX_11_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2K", "%", 0.0f, 100.0f);
		configParam(MIX_11_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3K", "%", 0.0f, 100.0f);
		configParam(MIX_11_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4K", "%", 0.0f, 100.0f);
		configParam(MIX_11_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5K", "%", 0.0f, 100.0f);
		configParam(MIX_11_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6K", "%", 0.0f, 100.0f);
		configParam(MIX_11_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7K", "%", 0.0f, 100.0f);
		configParam(MIX_11_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8K", "%", 0.0f, 100.0f);
		configParam(MIX_11_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9K", "%", 0.0f, 100.0f);
		configParam(MIX_11_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10K", "%", 0.0f, 100.0f);
		configParam(MIX_11_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11K", "%", 0.0f, 100.0f);
		configParam(MIX_11_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12K", "%", 0.0f, 100.0f);
		configParam(MIX_11_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13K", "%", 0.0f, 100.0f);
		configParam(MIX_11_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14K", "%", 0.0f, 100.0f);
		configParam(MIX_11_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15K", "%", 0.0f, 100.0f);
		configParam(MIX_11_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16K", "%", 0.0f, 100.0f);

		configParam(MIX_12_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1L", "%", 0.0f, 100.0f);
		configParam(MIX_12_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2L", "%", 0.0f, 100.0f);
		configParam(MIX_12_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3L", "%", 0.0f, 100.0f);
		configParam(MIX_12_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4L", "%", 0.0f, 100.0f);
		configParam(MIX_12_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5L", "%", 0.0f, 100.0f);
		configParam(MIX_12_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6L", "%", 0.0f, 100.0f);
		configParam(MIX_12_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7L", "%", 0.0f, 100.0f);
		configParam(MIX_12_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8L", "%", 0.0f, 100.0f);
		configParam(MIX_12_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9L", "%", 0.0f, 100.0f);
		configParam(MIX_12_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10L", "%", 0.0f, 100.0f);
		configParam(MIX_12_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11L", "%", 0.0f, 100.0f);
		configParam(MIX_12_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12L", "%", 0.0f, 100.0f);
		configParam(MIX_12_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13L", "%", 0.0f, 100.0f);
		configParam(MIX_12_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14L", "%", 0.0f, 100.0f);
		configParam(MIX_12_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15L", "%", 0.0f, 100.0f);
		configParam(MIX_12_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16L", "%", 0.0f, 100.0f);

		configParam(MIX_13_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1M", "%", 0.0f, 100.0f);
		configParam(MIX_13_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2M", "%", 0.0f, 100.0f);
		configParam(MIX_13_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3M", "%", 0.0f, 100.0f);
		configParam(MIX_13_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4M", "%", 0.0f, 100.0f);
		configParam(MIX_13_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5M", "%", 0.0f, 100.0f);
		configParam(MIX_13_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6M", "%", 0.0f, 100.0f);
		configParam(MIX_13_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7M", "%", 0.0f, 100.0f);
		configParam(MIX_13_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8M", "%", 0.0f, 100.0f);
		configParam(MIX_13_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9M", "%", 0.0f, 100.0f);
		configParam(MIX_13_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10M", "%", 0.0f, 100.0f);
		configParam(MIX_13_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11M", "%", 0.0f, 100.0f);
		configParam(MIX_13_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12M", "%", 0.0f, 100.0f);
		configParam(MIX_13_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13M", "%", 0.0f, 100.0f);
		configParam(MIX_13_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14M", "%", 0.0f, 100.0f);
		configParam(MIX_13_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15M", "%", 0.0f, 100.0f);
		configParam(MIX_13_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16M", "%", 0.0f, 100.0f);

		configParam(MIX_14_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1N", "%", 0.0f, 100.0f);
		configParam(MIX_14_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2N", "%", 0.0f, 100.0f);
		configParam(MIX_14_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3N", "%", 0.0f, 100.0f);
		configParam(MIX_14_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4N", "%", 0.0f, 100.0f);
		configParam(MIX_14_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5N", "%", 0.0f, 100.0f);
		configParam(MIX_14_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6N", "%", 0.0f, 100.0f);
		configParam(MIX_14_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7N", "%", 0.0f, 100.0f);
		configParam(MIX_14_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8N", "%", 0.0f, 100.0f);
		configParam(MIX_14_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9N", "%", 0.0f, 100.0f);
		configParam(MIX_14_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10N", "%", 0.0f, 100.0f);
		configParam(MIX_14_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11N", "%", 0.0f, 100.0f);
		configParam(MIX_14_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12N", "%", 0.0f, 100.0f);
		configParam(MIX_14_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13N", "%", 0.0f, 100.0f);
		configParam(MIX_14_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14N", "%", 0.0f, 100.0f);
		configParam(MIX_14_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15N", "%", 0.0f, 100.0f);
		configParam(MIX_14_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16N", "%", 0.0f, 100.0f);

		configParam(MIX_15_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1O", "%", 0.0f, 100.0f);
		configParam(MIX_15_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2O", "%", 0.0f, 100.0f);
		configParam(MIX_15_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3O", "%", 0.0f, 100.0f);
		configParam(MIX_15_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4O", "%", 0.0f, 100.0f);
		configParam(MIX_15_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5O", "%", 0.0f, 100.0f);
		configParam(MIX_15_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6O", "%", 0.0f, 100.0f);
		configParam(MIX_15_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7O", "%", 0.0f, 100.0f);
		configParam(MIX_15_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8O", "%", 0.0f, 100.0f);
		configParam(MIX_15_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9O", "%", 0.0f, 100.0f);
		configParam(MIX_15_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10O", "%", 0.0f, 100.0f);
		configParam(MIX_15_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11O", "%", 0.0f, 100.0f);
		configParam(MIX_15_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12O", "%", 0.0f, 100.0f);
		configParam(MIX_15_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13O", "%", 0.0f, 100.0f);
		configParam(MIX_15_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14O", "%", 0.0f, 100.0f);
		configParam(MIX_15_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15O", "%", 0.0f, 100.0f);
		configParam(MIX_15_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16O", "%", 0.0f, 100.0f);

		configParam(MIX_16_1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1P", "%", 0.0f, 100.0f);
		configParam(MIX_16_2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2P", "%", 0.0f, 100.0f);
		configParam(MIX_16_3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3P", "%", 0.0f, 100.0f);
		configParam(MIX_16_4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4P", "%", 0.0f, 100.0f);
		configParam(MIX_16_5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5P", "%", 0.0f, 100.0f);
		configParam(MIX_16_6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6P", "%", 0.0f, 100.0f);
		configParam(MIX_16_7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7P", "%", 0.0f, 100.0f);
		configParam(MIX_16_8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8P", "%", 0.0f, 100.0f);
		configParam(MIX_16_9_PARAM, -1.0f, 1.0f, 0.0f, "Mix 9P", "%", 0.0f, 100.0f);
		configParam(MIX_16_10_PARAM, -1.0f, 1.0f, 0.0f, "Mix 10P", "%", 0.0f, 100.0f);
		configParam(MIX_16_11_PARAM, -1.0f, 1.0f, 0.0f, "Mix 11P", "%", 0.0f, 100.0f);
		configParam(MIX_16_12_PARAM, -1.0f, 1.0f, 0.0f, "Mix 12P", "%", 0.0f, 100.0f);
		configParam(MIX_16_13_PARAM, -1.0f, 1.0f, 0.0f, "Mix 13P", "%", 0.0f, 100.0f);
		configParam(MIX_16_14_PARAM, -1.0f, 1.0f, 0.0f, "Mix 14P", "%", 0.0f, 100.0f);
		configParam(MIX_16_15_PARAM, -1.0f, 1.0f, 0.0f, "Mix 15P", "%", 0.0f, 100.0f);
		configParam(MIX_16_16_PARAM, -1.0f, 1.0f, 0.0f, "Mix 16P", "%", 0.0f, 100.0f);
	}
};

} // namespace bogaudio
