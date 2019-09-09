#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelVU;

namespace bogaudio {

struct VU : BGModule {
	enum ParamsIds {
		NUM_PARAMS
	};

	enum InputsIds {
		L_INPUT,
		R_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		L_OUTPUT,
		R_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	RootMeanSquare _lRms;
	RootMeanSquare _rRms;
	float _lLevel = 0.0f;
	float _rLevel = 0.0f;

	RunningAverage _lPeakRms;
	RunningAverage _rPeakRms;
	bogaudio::dsp::SlewLimiter _lPeakSlew;
	bogaudio::dsp::SlewLimiter _rPeakSlew;
	float _lPeakFalling = 0.0f;
	float _rPeakFalling = 0.0f;
	float _lPeakLevel = 0.0f;
	float _rPeakLevel = 0.0f;

	VU() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		sampleRateChange();
		_lRms.setSensitivity(1.0f);
		_rRms.setSensitivity(1.0f);
		_lPeakRms.setSensitivity(0.025f);
		_rPeakRms.setSensitivity(0.025f);
	}

	void sampleRateChange() override;
	void processChannel(const ProcessArgs& args, int _c) override;
};

} // namespace bogaudio
