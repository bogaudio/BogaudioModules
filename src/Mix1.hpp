#pragma once

#include "bogaudio.hpp"
#include "mixer.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMix1;

namespace bogaudio {

struct Mix1 : LinearCVMixerModule {
	enum ParamsIds {
		LEVEL_PARAM,
		MUTE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		MUTE_INPUT,
		LEVEL_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	MixerChannel* _engines[maxChannels] {};
	float _rmsSum = 0.0f;
	float _rms = 0.0f;

	Mix1() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(LEVEL_PARAM, 0.0f, 1.0f, fabsf(MixerChannel::minDecibels) / (MixerChannel::maxDecibels - MixerChannel::minDecibels), "Level", "dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configSwitch(MUTE_PARAM, 0.0f, 1.0f, 0.0f, "Mute", {"Unmuted", "Muted"});
		configBypass(IN_INPUT, OUT_OUTPUT);

		configInput(MUTE_INPUT, "Mute CV");
		configInput(LEVEL_INPUT, "Level CV");
		configInput(IN_INPUT, "Signal");

		configOutput(OUT_OUTPUT, "OUT");
	}

	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void postProcessAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
