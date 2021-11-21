#pragma once

#include "bogaudio.hpp"
#include "vco_base.hpp"

extern Model* modelSine;

namespace bogaudio {

struct Sine : VCOBase {
	enum ParamsIds {
		FREQUENCY_PARAM,
		SLOW_PARAM,
		FM_DEPTH_PARAM,
		PHASE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		PITCH_INPUT,
		FM_INPUT,
		PHASE_INPUT,
		SYNC_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum Wave {
		SINE_WAVE,
		TRIANGLE_WAVE,
		SAW_WAVE,
		RAMP_WAVE,
		SQUARE_WAVE,
		PULSE_25_WAVE,
		PULSE_10_WAVE
	};

	Wave _wave = SINE_WAVE;
	float _outputScale = 1.0f;

	Sine()
	: VCOBase(
		FREQUENCY_PARAM,
		-1,
		PITCH_INPUT,
		SYNC_INPUT,
		FM_INPUT
	)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<VCOFrequencyParamQuantity>(FREQUENCY_PARAM, -3.0f, 6.0f, 0.0f, "Frequency", " Hz");
		configButton(SLOW_PARAM, "Slow mode");
		configParam(FM_DEPTH_PARAM, 0.0f, 1.0f, 0.0f, "FM depth", "%", 0.0f, 100.0f);
		configParam(PHASE_PARAM, -1.0f, 1.0f, 0.0f, "Phase offset", "º", 0.0f, 180.0f);

		configInput(PITCH_INPUT, "Pitch (1V/octave)");
		configInput(FM_INPUT, "Frequency modulation");
		configInput(PHASE_INPUT, "Phase CV");
		configInput(SYNC_INPUT, "Sync");

		configOutput(OUT_OUTPUT, "Signal");
	}

	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	bool active() override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
