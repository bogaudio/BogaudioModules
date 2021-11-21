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

	bool _flipped1[maxChannels] {};
	bool _flipped2[maxChannels] {};
	PositiveZeroCrossing _trigger1[maxChannels];
	Trigger _resetTrigger1[maxChannels];
	PositiveZeroCrossing _trigger2[maxChannels];
	Trigger _resetTrigger2[maxChannels];

	FlipFlop() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

		configInput(IN1_INPUT, "Trigger 1");
		configInput(RESET1_INPUT, "Reset 1");
		configInput(IN2_INPUT, "Trigger 2");
		configInput(RESET2_INPUT, "Reset 2");

		configOutput(A1_OUTPUT, "A1 gate");
		configOutput(B1_OUTPUT, "B1 gate");
		configOutput(A2_OUTPUT, "A2 gate");
		configOutput(B2_OUTPUT, "B2 gate");
	}

	void reset() override;
	void processAll(const ProcessArgs& args) override;
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
