#pragma once

#include "bogaudio.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/pitch.hpp"

using namespace bogaudio::dsp;

extern Model* modelAdditator;

namespace bogaudio {

struct Additator : Module {
	enum ParamsIds {
		FREQUENCY_PARAM,
		PARTIALS_PARAM,
		WIDTH_PARAM,
		EVEN_WIDTH_PARAM,
		DECAY_PARAM,
		ODD_SCALE_PARAM,
		EVEN_SCALE_PARAM,
		PHASE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		PITCH_INPUT,
		SYNC_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	const int maxPartials = 100;
	int _partials = 0;
	float _width = 0.0f;
	float _evenWidth = 0.0f;
	float _decay = 0.0f;
	float _oddScale = 0.0f;
	float _evenScale = 0.0f;
	float _phase = 0.0f;
	SineBankOscillator _oscillator;
	SchmittTrigger _syncTrigger;

	Additator()
	: Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	, _oscillator(engineGetSampleRate(), 1000.0, maxPartials)
	{
		onReset();
	}

	virtual void onReset() override;
	virtual void onSampleRateChange() override;
	virtual void step() override;
};

} // namespace bogaudio
