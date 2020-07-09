#pragma once

#include "bogaudio.hpp"
#include "follower_base.hpp"
#include "dsp/filters/equalizer.hpp"
#include "dsp/filters/utility.hpp"

using namespace bogaudio::dsp;

extern Model* modelFollow;

namespace bogaudio {

struct Follow : FollowerBase {
	enum ParamsIds {
		RESPONSE_PARAM,
		GAIN_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		RESPONSE_INPUT,
		GAIN_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		EnvelopeFollower follower;
		Amplifier gain;
	};

	Engine* _engines[maxChannels] {};
	Saturator _saturator;

	Follow() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(RESPONSE_PARAM, 0.0f, 1.0f, 0.3f, "Smoothing", "%", 0.0f, 100.0f);
		configParam<EFGainParamQuantity>(GAIN_PARAM, -1.0f, 1.0f, 0.0f, "Gain", " dB");
	}

	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
