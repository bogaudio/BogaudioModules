#pragma once

#include "bogaudio.hpp"
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
		configParam<AmpliferParamQuantity>(LEVEL_PARAM, 0.0f, 1.0f, 1.0f, "Output level");
	}

	void sampleRateChange() override;
	void modulate() override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
