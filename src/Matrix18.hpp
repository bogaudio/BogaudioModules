#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"

extern Model* modelMatrix18;

namespace bogaudio {

struct Matrix18 : KnobMatrixModule {
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

	Matrix18() : KnobMatrixModule(1, 8, MIX1_PARAM, IN_INPUT, OUT1_OUTPUT) {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(MIX1_PARAM, -1.0f, 1.0f, 0.0f, "Route 1", "%", 0.0f, 100.0f);
		configParam(MIX2_PARAM, -1.0f, 1.0f, 0.0f, "Route 2", "%", 0.0f, 100.0f);
		configParam(MIX3_PARAM, -1.0f, 1.0f, 0.0f, "Route 3", "%", 0.0f, 100.0f);
		configParam(MIX4_PARAM, -1.0f, 1.0f, 0.0f, "Route 4", "%", 0.0f, 100.0f);
		configParam(MIX5_PARAM, -1.0f, 1.0f, 0.0f, "Route 5", "%", 0.0f, 100.0f);
		configParam(MIX6_PARAM, -1.0f, 1.0f, 0.0f, "Route 6", "%", 0.0f, 100.0f);
		configParam(MIX7_PARAM, -1.0f, 1.0f, 0.0f, "Route 7", "%", 0.0f, 100.0f);
		configParam(MIX8_PARAM, -1.0f, 1.0f, 0.0f, "Route 8", "%", 0.0f, 100.0f);

		configInput(IN_INPUT, "Signal");

		configOutput(OUT1_OUTPUT, "Signal 1");
		configOutput(OUT2_OUTPUT, "Signal 2");
		configOutput(OUT3_OUTPUT, "Signal 3");
		configOutput(OUT4_OUTPUT, "Signal 4");
		configOutput(OUT5_OUTPUT, "Signal 5");
		configOutput(OUT6_OUTPUT, "Signal 6");
		configOutput(OUT7_OUTPUT, "Signal 7");
		configOutput(OUT8_OUTPUT, "Signal 8");
	}
};

} // namespace bogaudio
