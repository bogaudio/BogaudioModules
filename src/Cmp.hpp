#pragma once

#include "bogaudio.hpp"

extern Model* modelCmp;

namespace bogaudio {

struct Cmp : BGModule {
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

	enum State {
		LOW,
		HIGH,
		LAG_LOW,
		LAG_HIGH
	};

	State _thresholdState[maxChannels] {};
	int _thresholdLag[maxChannels] {};
	State _windowState[maxChannels] {};
	int _windowLag[maxChannels] {};
	float _highValue = 10.0f;
	float _lowValue = 0.0f;
	int _lagInSamples[maxChannels] {};

	Cmp() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(A_PARAM, -1.0f, 1.0f, 0.0f, "A", " V", 0.0f, 10.0f);
		configParam(B_PARAM, -1.0f, 1.0f, 0.0f, "B", " V", 0.0f, 10.0f);
		configParam(WINDOW_PARAM, 0.0f, 1.0f, 0.5f, "Window", " V", 0.0f, 10.0f);
		configParam<ScaledSquaringParamQuantity<1>>(LAG_PARAM, 0.0f, 1.0f, 0.1f, "Lag", " s");
		configParam(OUTPUT_PARAM, 0.0f, 1.0f, 0.0f, "Output");

		reset();
	}

	void reset() override;
	bool active() override;
	int channels() override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void stepChannel(
		int c,
		bool high,
		State& state,
		int& channelLag,
		Output& highOutput,
		Output& lowOutput
	);
};

} // namespace bogaudio
