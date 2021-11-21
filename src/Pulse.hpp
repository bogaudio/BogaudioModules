#pragma once

#include "bogaudio.hpp"
#include "vco_base.hpp"

extern Model* modelPulse;

namespace bogaudio {

struct Pulse : VCOBase {
	enum ParamsIds {
		FREQUENCY_PARAM,
		SLOW_PARAM,
		PW_PARAM,
		PWM_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		PITCH_INPUT,
		PWM_INPUT,
		SYNC_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	Pulse()
	: VCOBase(
		FREQUENCY_PARAM,
		-1,
		PITCH_INPUT,
		SYNC_INPUT,
		-1
	)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<VCOFrequencyParamQuantity>(FREQUENCY_PARAM, -3.0f, 6.0f, 0.0f, "Frequency", " Hz");
		configButton(SLOW_PARAM, "Slow mode");
		configParam(PW_PARAM, -1.0f, 1.0f, 0.0f, "Pulse width", "%", 0.0f, 100.0f*0.5f*(1.0f - 2.0f * SquareOscillator::minPulseWidth), 50.0f);
		configParam(PWM_PARAM, -1.0f, 1.0f, 0.0f, "Pulse width CV amount");

		configInput(PITCH_INPUT, "Pitch (1V/octave)");
		configInput(PWM_INPUT, "Pulse width CV");
		configInput(SYNC_INPUT, "Sync");

		configOutput(OUT_OUTPUT, "Pulse");
	}

	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	bool active() override;
	void addChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
