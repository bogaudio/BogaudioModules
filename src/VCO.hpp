#pragma once

#include "bogaudio.hpp"
#include "vco_base.hpp"

extern Model* modelVCO;

namespace bogaudio {

struct VCO : VCOBase {
	enum ParamsIds {
		FREQUENCY_PARAM,
		FINE_PARAM,
		SLOW_PARAM,
		PW_PARAM,
		FM_PARAM,
		FM_TYPE_PARAM,
		LINEAR_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		PITCH_INPUT,
		SYNC_INPUT,
		PW_INPUT,
		FM_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		SQUARE_OUTPUT,
		SAW_OUTPUT,
		TRIANGLE_OUTPUT,
		SINE_OUTPUT,
		NUM_OUTPUTS
	};

	VCO()
	: VCOBase(
		FREQUENCY_PARAM,
		FINE_PARAM,
		PITCH_INPUT,
		SYNC_INPUT,
		FM_INPUT
	)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<VCOFrequencyParamQuantity>(FREQUENCY_PARAM, -3.0f, 6.0f, 0.0f, "Frequency", " Hz");
		configParam(FINE_PARAM, -1.0f, 1.0f, 0.0f, "Fine tune", " cents", 0.0f, 100.0f);
		configButton(SLOW_PARAM, "Slow mode");
		configParam(PW_PARAM, -1.0f, 1.0f, 0.0f, "Pulse width", "%", 0.0f, 100.0f*0.5f*(1.0f - 2.0f * SquareOscillator::minPulseWidth), 50.0f);
		configParam(FM_PARAM, 0.0f, 1.0f, 0.0f, "FM depth", "%", 0.0f, 100.0f);
		configSwitch(FM_TYPE_PARAM, 0.0f, 1.0f, 1.0f, "FM mode", {"Linear FM", "Exponential FM"});
		configButton(LINEAR_PARAM, "Linear frequency");

		configInput(PITCH_INPUT, "Pitch (1V/octave)");
		configInput(SYNC_INPUT, "Sync");
		configInput(PW_INPUT, "Pulse width CV");
		configInput(FM_INPUT, "Frequency modulation");

		configOutput(SQUARE_OUTPUT, "Square signal");
		configOutput(SAW_OUTPUT, "Saw signal");
		configOutput(TRIANGLE_OUTPUT, "Triangle signal");
		configOutput(SINE_OUTPUT, "Sine signal");
	}

	bool active() override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
