#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/utility.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelMono;

namespace bogaudio {

struct Mono : BGModule {
	enum ParamsIds {
		COMPRESSION_PARAM,
		LEVEL_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		POLY_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		MONO_OUTPUT,
		NUM_OUTPUTS
	};

	RootMeanSquare _channelRMSs[maxChannels];
	float _channelLevels[maxChannels] {};
	RootMeanSquare _detectorRMS;
	bogaudio::dsp::SlewLimiter _attackSL, _releaseSL;
	Compressor _compressor;
	Amplifier _compAmp, _levelAmp;
	Saturator _saturator;
	int _activeChannels = 0;
	float _ratio = 2.0f;
	float _releaseMS = 200.0f;
	float _lastEnv = 0.0f;
	float _compressionDb = 0.0f;

	Mono() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(COMPRESSION_PARAM, 0.0f, 1.0f, 0.2f, "Compression", "", 0.0f, 10.0f);
		configParam<AmplifierParamQuantity>(LEVEL_PARAM, 0.0f, 1.0f, 1.0f, "Output level");

		configInput(POLY_INPUT, "POLY");

		configOutput(MONO_OUTPUT, "MONO");
	}

	void sampleRateChange() override;
	void modulate() override;
	void processAll(const ProcessArgs& args) override;
};

} // namespace bogaudio
