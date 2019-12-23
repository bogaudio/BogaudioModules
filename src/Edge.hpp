#pragma once

#include "bogaudio.hpp"
#include "signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelEdge;

namespace bogaudio {

struct Edge : BGModule {
	enum ParamsIds {
		RISE_PARAM,
		FALL_PARAM,
		HOLD_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		HIGH_OUTPUT,
		RISE_OUTPUT,
		FALL_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		HIGH_LIGHT,
		NUM_LIGHTS
	};

	enum State {
		LOW_STATE,
		LOW2_STATE,
		HIGH_STATE,
		HIGH2_STATE
	};

	float _riseThreshold = 0.0f;
	float _fallThreshold = 0.0f;
	float _holdSeconds = 0.0f;
	State _state[maxChannels] {};
	Timer _timer[maxChannels];
	rack::dsp::PulseGenerator _riseOutputPulseGen[maxChannels];
	rack::dsp::PulseGenerator _fallOutputPulseGen[maxChannels];
	int _highLightSum = 0;

	Edge() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(RISE_PARAM, -1.0f, 1.0f, 0.1f, "Rising threshold", " V", 0.0f, 10.0f);
		configParam(FALL_PARAM, -1.0f, 1.0f, 0.01f, "Falling threshold", " V", 0.0f, 10.0f);
		configParam<ScaledSquaringParamQuantity<1000>>(HOLD_PARAM, 0.0f, 1.0f, 0.031623f, "Hold/reset time", " ms");
	}

	void reset() override;
	int channels() override;
	void addChannel(int c) override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void postProcess(const ProcessArgs& args) override;
};

} // namespace bogaudio
