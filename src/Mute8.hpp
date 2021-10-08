#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMute8;

namespace bogaudio {

struct Mute8 : BGModule {
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
		INPUT1_INPUT,
		INPUT2_INPUT,
		INPUT3_INPUT,
		INPUT4_INPUT,
		INPUT5_INPUT,
		INPUT6_INPUT,
		INPUT7_INPUT,
		INPUT8_INPUT,
		MUTE1_INPUT,
		MUTE2_INPUT,
		MUTE3_INPUT,
		MUTE4_INPUT,
		MUTE5_INPUT,
		MUTE6_INPUT,
		MUTE7_INPUT,
		MUTE8_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUTPUT1_OUTPUT,
		OUTPUT2_OUTPUT,
		OUTPUT3_OUTPUT,
		OUTPUT4_OUTPUT,
		OUTPUT5_OUTPUT,
		OUTPUT6_OUTPUT,
		OUTPUT7_OUTPUT,
		OUTPUT8_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		MUTE1_LIGHT,
		MUTE2_LIGHT,
		MUTE3_LIGHT,
		MUTE4_LIGHT,
		MUTE5_LIGHT,
		MUTE6_LIGHT,
		MUTE7_LIGHT,
		MUTE8_LIGHT,
		NUM_LIGHTS
	};

	static const float maxDecibels;
	static const float minDecibels;
	static const float slewTimeMS;

	Amplifier _amplifiers[8][maxChannels];
	bogaudio::dsp::SlewLimiter _slewLimiters[8][maxChannels];
	Trigger _triggers[8][maxChannels];
	bool _latches[8][maxChannels] {};
	bool _latchingCVs = false;

	Mute8() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configSwitch(MUTE1_PARAM, 0.0f, 3.0f, 0.0f, "Mute 1", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE2_PARAM, 0.0f, 3.0f, 0.0f, "Mute 2", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE3_PARAM, 0.0f, 3.0f, 0.0f, "Mute 3", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE4_PARAM, 0.0f, 3.0f, 0.0f, "Mute 4", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE5_PARAM, 0.0f, 3.0f, 0.0f, "Mute 5", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE6_PARAM, 0.0f, 3.0f, 0.0f, "Mute 6", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE7_PARAM, 0.0f, 3.0f, 0.0f, "Mute 7", {"Unmuted", "Muted", "Soloed", "Soloed"});
		configSwitch(MUTE8_PARAM, 0.0f, 3.0f, 0.0f, "Mute 8", {"Unmuted", "Muted", "Soloed", "Soloed"});

		configBypass(INPUT1_INPUT, OUTPUT1_OUTPUT);
		configBypass(INPUT2_INPUT, OUTPUT2_OUTPUT);
		configBypass(INPUT3_INPUT, OUTPUT3_OUTPUT);
		configBypass(INPUT4_INPUT, OUTPUT4_OUTPUT);
		configBypass(INPUT5_INPUT, OUTPUT5_OUTPUT);
		configBypass(INPUT6_INPUT, OUTPUT6_OUTPUT);
		configBypass(INPUT7_INPUT, OUTPUT7_OUTPUT);
		configBypass(INPUT8_INPUT, OUTPUT8_OUTPUT);

		configInput(INPUT1_INPUT, "INPUT1");
		configInput(INPUT2_INPUT, "INPUT2");
		configInput(INPUT3_INPUT, "INPUT3");
		configInput(INPUT4_INPUT, "INPUT4");
		configInput(INPUT5_INPUT, "INPUT5");
		configInput(INPUT6_INPUT, "INPUT6");
		configInput(INPUT7_INPUT, "INPUT7");
		configInput(INPUT8_INPUT, "INPUT8");
		configInput(MUTE1_INPUT, "MUTE1");
		configInput(MUTE2_INPUT, "MUTE2");
		configInput(MUTE3_INPUT, "MUTE3");
		configInput(MUTE4_INPUT, "MUTE4");
		configInput(MUTE5_INPUT, "MUTE5");
		configInput(MUTE6_INPUT, "MUTE6");
		configInput(MUTE7_INPUT, "MUTE7");
		configInput(MUTE8_INPUT, "MUTE8");

		configOutput(OUTPUT1_OUTPUT, "OUTPUT1");
		configOutput(OUTPUT2_OUTPUT, "OUTPUT2");
		configOutput(OUTPUT3_OUTPUT, "OUTPUT3");
		configOutput(OUTPUT4_OUTPUT, "OUTPUT4");
		configOutput(OUTPUT5_OUTPUT, "OUTPUT5");
		configOutput(OUTPUT6_OUTPUT, "OUTPUT6");
		configOutput(OUTPUT7_OUTPUT, "OUTPUT7");
		configOutput(OUTPUT8_OUTPUT, "OUTPUT8");
	}

	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	void reset() override;
	void sampleRateChange() override;
	void processAll(const ProcessArgs& args) override;
	void stepChannel(int i, bool solo);
};

} // namespace bogaudio
