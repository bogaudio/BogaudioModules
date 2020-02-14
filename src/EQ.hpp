#pragma once

#include "bogaudio.hpp"
#include "filter.hpp"

extern Model* modelEQ;

namespace bogaudio {

struct EQ : BGModule {
	enum ParamsIds {
		LOW_PARAM,
		MID_PARAM,
		HIGH_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	typedef bogaudio::dsp::Equalizer Engine;

	float _lowDb = 0.0f;
	float _midDb = 0.0f;
	float _highDb = 0.0f;
	Engine* _engines[maxChannels] {};

	EQ() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(LOW_PARAM, Engine::cutDb, Engine::gainDb, 0.0f, "Low", " dB");
		configParam(MID_PARAM, Engine::cutDb, Engine::gainDb, 0.0f, "Mid", " dB");
		configParam(HIGH_PARAM, Engine::cutDb, Engine::gainDb, 0.0f, "High", " dB");
	}

	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
