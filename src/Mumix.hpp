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
	Amplifier _amplifiers[8];
	bogaudio::dsp::SlewLimiter _slewLimiters[8];
	bool _muted[8] {};
	float _invActive = 0.0f;

	Mumix() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configSwitch(MUTE1_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE2_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE3_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE4_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE5_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE6_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE7_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE8_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8", {"Unmuted", "Muted", "Soloed", "Soloed"});

		configInput(IN1_INPUT, "IN1");
		configInput(IN2_INPUT, "IN2");
		configInput(IN3_INPUT, "IN3");
		configInput(IN4_INPUT, "IN4");
		configInput(IN5_INPUT, "IN5");
		configInput(IN6_INPUT, "IN6");
		configInput(IN7_INPUT, "IN7");
		configInput(IN8_INPUT, "IN8");

		configOutput(OUT_OUTPUT, "OUT");
	}

	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void modulate() override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
