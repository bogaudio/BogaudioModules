#pragma once

#include "bogaudio.hpp"
#include "matrix_base.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelUMix;

namespace bogaudio {

struct UMix : MatrixBaseModule {
	enum ParamsIds {
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

	Saturator _saturator[maxChannels];

	UMix() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
	}

	bool active() override;
	int channels() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
