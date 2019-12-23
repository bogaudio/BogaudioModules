#pragma once

#include "bogaudio.hpp"

extern Model* modelAssign;

namespace bogaudio {

struct Assign : BGModule {
	enum ParamsIds {
		CHANNELS_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		PITCH_INPUT,
		GATE_INPUT,
		RESET_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		PITCH_OUTPUT,
		GATE_OUTPUT,
		NUM_OUTPUTS
	};

	int _channelsOut;
	Trigger _resetTrigger;
	Trigger _gateTrigger[maxChannels];
	bool _gateHigh[maxChannels] {};
	int _pitchInAssignment[maxChannels] {};
	int _gateInAssignment[maxChannels] {};
	int _pitchOutAssignment[maxChannels] {};
	int _gateOutAssignment[maxChannels] {};
	float _lastPitchOut[maxChannels] {};
	unsigned long _assignedAtStep[maxChannels] {};
	int _nextAssign = 0;
	unsigned long _step = 0;

	Assign() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(CHANNELS_PARAM, 1.0f, 16.0f, 0.0f, "Channels");
		reset();
	}

	void reset() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void processChannel(const ProcessArgs& args, int c) override {};
	void postProcess(const ProcessArgs& args) override;
};

} // namespace bogaudio
