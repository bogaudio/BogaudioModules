#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/equalizer.hpp"

extern Model* modelEQS;

namespace bogaudio {

struct EQS : BGModule {
	enum ParamsIds {
		LOW_PARAM,
		MID_PARAM,
		HIGH_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		LEFT_INPUT,
		RIGHT_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		LEFT_OUTPUT,
		RIGHT_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		bogaudio::dsp::Equalizer _left;
		bogaudio::dsp::Equalizer _right;
	};

	float _lowDb = 0.0f;
	float _midDb = 0.0f;
	float _highDb = 0.0f;
	Engine* _engines[maxChannels] {};

	EQS() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<EQParamQuantity>(LOW_PARAM, -1.0f, 1.0f, 0.0f, "Low", " dB");
		configParam<EQParamQuantity>(MID_PARAM, -1.0f, 1.0f, 0.0f, "Mid", " dB");
		configParam<EQParamQuantity>(HIGH_PARAM, -1.0f, 1.0f, 0.0f, "High", " dB");
	}

	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	float knobToDb(Param& p);
	void modulate() override;
	void modulateChannel(int c) override;
	void processAll(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
