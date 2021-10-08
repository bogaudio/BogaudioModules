#pragma once

#include "bogaudio.hpp"
#include "dsp/noise.hpp"
#include "poly_channels.hpp"

using namespace bogaudio::dsp;

extern Model* modelNoise;

namespace bogaudio {

struct Noise : PolyChannelsModule {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		ABS_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		WHITE_OUTPUT,
		PINK_OUTPUT,
		RED_OUTPUT,
		GAUSS_OUTPUT,
		ABS_OUTPUT,
		BLUE_OUTPUT,
		NUM_OUTPUTS
	};

	BlueNoiseGenerator _blue;
	WhiteNoiseGenerator _white;
	PinkNoiseGenerator _pink;
	RedNoiseGenerator _red;
	GaussianNoiseGenerator _gauss;

	Noise() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);

		configInput(ABS_INPUT, "ABS");

		configOutput(WHITE_OUTPUT, "WHITE");
		configOutput(PINK_OUTPUT, "PINK");
		configOutput(RED_OUTPUT, "RED");
		configOutput(GAUSS_OUTPUT, "GAUSS");
		configOutput(ABS_OUTPUT, "ABS");
		configOutput(BLUE_OUTPUT, "BLUE");
	}

	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
