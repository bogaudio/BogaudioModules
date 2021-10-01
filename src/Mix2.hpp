#pragma once

#include "bogaudio.hpp"
#include "mixer.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMix2;

namespace bogaudio {

struct Mix2 : LinearCVMixerModule {
	enum ParamsIds {
		LEVEL_PARAM,
		MUTE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		LEVEL_INPUT,
		MUTE_INPUT,
		L_INPUT,
		R_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		L_OUTPUT,
		R_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		MixerChannel left;
		MixerChannel right;

		Engine(
			Param& levelParam,
			Param& muteParam,
			Input& levelInput,
			Input& muteInput
		)
		: left(levelParam, muteParam, levelInput, 1000.0f, &muteInput)
		, right(levelParam, muteParam, levelInput, 1000.0f, &muteInput)
		{
			float sr = APP->engine->getSampleRate();
			left.setSampleRate(sr);
			right.setSampleRate(sr);
		}
	};

	Engine* _engines[maxChannels] {};
	float _leftRmsSum = 0.0f;
	float _leftRms = 0.0f;
	float _rightRmsSum = 0.0f;
	float _rightRms = 0.0f;

	Mix2() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(LEVEL_PARAM, 0.0f, 1.0f, fabsf(MixerChannel::minDecibels) / (MixerChannel::maxDecibels - MixerChannel::minDecibels), "Level", "dB", 0.0f, MixerChannel::maxDecibels - MixerChannel::minDecibels, MixerChannel::minDecibels);
		configParam(MUTE_PARAM, 0.0f, 1.0f, 0.0f, "Mute");
	}

	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void postProcessAlways(const ProcessArgs& args) override;
	void processBypass(const ProcessArgs& args) override;
};

} // namespace bogaudio
