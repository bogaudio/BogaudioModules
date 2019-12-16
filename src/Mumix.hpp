#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMumix;

namespace bogaudio {

struct Mumix : MatrixBaseModule {
	enum ParamsIds {
		MUTE1_PARAM,
		MUTE2_PARAM,
		MUTE3_PARAM,
		MUTE4_PARAM,
		MUTE5_PARAM,
		MUTE6_PARAM,
		MUTE7_PARAM,
		MUTE8_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN1_INPUT,
		IN2_INPUT,
		IN3_INPUT,
		IN4_INPUT,
		IN5_INPUT,
		IN6_INPUT,
		IN7_INPUT,
		IN8_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	static const float maxDecibels;
	static const float minDecibels;
	static const float slewTimeMS;

	Saturator _saturator[maxChannels];
	Amplifier _amplifiers[maxChannels];
	bogaudio::dsp::SlewLimiter _slewLimiters[maxChannels];
	bool _muted[8] {};

	Mumix() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(MUTE1_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1");
		configParam(MUTE2_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2");
		configParam(MUTE3_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3");
		configParam(MUTE4_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4");
		configParam(MUTE5_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5");
		configParam(MUTE6_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6");
		configParam(MUTE7_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7");
		configParam(MUTE8_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8");
	}

	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void modulate() override;
	void always(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
