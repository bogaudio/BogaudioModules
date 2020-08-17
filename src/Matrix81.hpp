#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"

extern Model* modelMatrix81;

namespace bogaudio {

struct Matrix81 : KnobMatrixModule {
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

	Matrix81() : KnobMatrixModule(8, 1, MIX1_PARAM, IN1_INPUT, OUT_OUTPUT) {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(MIX1_PARAM, -1.0f, 1.0f, 0.0f, "Mix 1", "%", 0.0f, 100.0f);
		configParam(MIX2_PARAM, -1.0f, 1.0f, 0.0f, "Mix 2", "%", 0.0f, 100.0f);
		configParam(MIX3_PARAM, -1.0f, 1.0f, 0.0f, "Mix 3", "%", 0.0f, 100.0f);
		configParam(MIX4_PARAM, -1.0f, 1.0f, 0.0f, "Mix 4", "%", 0.0f, 100.0f);
		configParam(MIX5_PARAM, -1.0f, 1.0f, 0.0f, "Mix 5", "%", 0.0f, 100.0f);
		configParam(MIX6_PARAM, -1.0f, 1.0f, 0.0f, "Mix 6", "%", 0.0f, 100.0f);
		configParam(MIX7_PARAM, -1.0f, 1.0f, 0.0f, "Mix 7", "%", 0.0f, 100.0f);
		configParam(MIX8_PARAM, -1.0f, 1.0f, 0.0f, "Mix 8", "%", 0.0f, 100.0f);
	}
};

} // namespace bogaudio
