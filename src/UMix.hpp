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

	bool active() override;
	int channels() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
