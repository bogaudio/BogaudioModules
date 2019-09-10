#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelFlipFlop;

namespace bogaudio {

struct FlipFlop : BGModule {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		IN1_INPUT,
		RESET1_INPUT,
		IN2_INPUT,
		RESET2_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		A1_OUTPUT,
		B1_OUTPUT,
		A2_OUTPUT,
		B2_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	bool _flipped1[maxChannels] {};
	bool _flipped2[maxChannels] {};
	PositiveZeroCrossing _trigger1[maxChannels];
	Trigger _resetTrigger1[maxChannels];
	PositiveZeroCrossing _trigger2[maxChannels];
	Trigger _resetTrigger2[maxChannels];

	FlipFlop() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		reset();
	}

	void reset() override;
	void processChannel(const ProcessArgs& args, int c) override;
	void channelStep(
		int c,
		int channels,
		Input& triggerInput,
		Input& resetInput,
		Output& aOutput,
		Output& bOutput,
		PositiveZeroCrossing* trigger,
		Trigger* resetTrigger,
		bool* flipped
	);
};

} // namespace bogaudio
