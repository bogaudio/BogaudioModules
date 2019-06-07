#pragma once

#include "bogaudio.hpp"

extern Model* modelCmp;

namespace bogaudio {

struct Cmp : Module {
	enum ParamsIds {
		A_PARAM,
		B_PARAM,
		WINDOW_PARAM,
		LAG_PARAM,
		OUTPUT_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		A_INPUT,
		B_INPUT,
		WINDOW_INPUT,
		LAG_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		GREATER_OUTPUT,
		LESS_OUTPUT,
		EQUAL_OUTPUT,
		NOT_EQUAL_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	enum State {
		LOW,
		HIGH,
		LAG_LOW,
		LAG_HIGH
	};

	State _thresholdState;
	int _thresholdLag = 0;
	State _windowState;
	int _windowLag = 0;

	Cmp() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(A_PARAM, -1.0f, 1.0f, 0.0f, "A", "V", 0.0f, 10.0f);
		configParam(B_PARAM, -1.0f, 1.0f, 0.0f, "B", "V", 0.0f, 10.0f);
		configParam(WINDOW_PARAM, 0.0f, 1.0f, 0.5f, "Window", "V", 0.0f, 10.0f);
		configParam(LAG_PARAM, 0.0f, 1.0f, 0.1f, "lag");
		configParam(OUTPUT_PARAM, 0.0f, 1.0f, 0.0f, "Output");

		onReset();
	}

	void onReset() override;
	void process(const ProcessArgs& args) override;
	void stepChannel(
		bool high,
		float highValue,
		float lowValue,
		State& state,
		int& channelLag,
		int& lag,
		Output& highOutput,
		Output& lowOutput
	);
	int lagInSamples();
};

} // namespace bogaudio
