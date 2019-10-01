#pragma once

#include "bogaudio.hpp"
#include "mixer.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMix1;

namespace bogaudio {

struct Mix1 : BGModule {
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

	enum LightsIds {
		NUM_LIGHTS
	};

	MixerChannel* _channel = NULL;

	Mix1() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(LEVEL_PARAM, 0.0f, 1.0f, fabsf(MixerChannel::minDecibels) / (MixerChannel::maxDecibels - MixerChannel::minDecibels), "Level", "dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(MUTE_PARAM, 0.0f, 1.0f, 0.0f, "Mute");

		_channel = new MixerChannel(
			params[LEVEL_PARAM],
			params[LEVEL_PARAM], // not used
			params[MUTE_PARAM],
			inputs[LEVEL_INPUT],
			inputs[LEVEL_INPUT], // not used
			1000.0f,
			&inputs[MUTE_INPUT]
		);
		sampleRateChange();
	}
	virtual ~Mix1() {
		delete _channel;
	}

	void sampleRateChange() override;
	void processChannel(const ProcessArgs& args, int _c) override;
};

} // namespace bogaudio
