#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelPan;

namespace bogaudio {

struct Pan : BGModule {
	enum ParamsIds {
		PAN1_PARAM,
		PAN2_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		CV1_INPUT,
		IN1_INPUT,
		CV2_INPUT,
		IN2_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		L_OUTPUT,
		R_OUTPUT,
		NUM_OUTPUTS
	};

	Panner _panner1[maxChannels];
	Panner _panner2[maxChannels];
	bogaudio::dsp::SlewLimiter _slew1[maxChannels];
	bogaudio::dsp::SlewLimiter _slew2[maxChannels];
	Saturator _saturatorLeft[maxChannels];
	Saturator _saturatorRight[maxChannels];

	Pan() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(PAN1_PARAM, -1.0f, 1.0f, 0.0f, "Panning 1", "%", 0.0f, 100.0f);
		configParam(PAN2_PARAM, -1.0f, 1.0f, 0.0f, "Panning 2", "%", 0.0f, 100.0f);

		configInput(CV1_INPUT, "Pan 1 CV");
		configInput(IN1_INPUT, "Signal 1");
		configInput(CV2_INPUT, "Pan 2 CV");
		configInput(IN2_INPUT, "Signal 2");

		configOutput(L_OUTPUT, "Left signal");
		configOutput(R_OUTPUT, "Right signal");

		sampleRateChange();
	}

	bool active() override;
	int channels() override;
	void sampleRateChange() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
