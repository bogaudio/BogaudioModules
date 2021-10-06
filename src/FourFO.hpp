#pragma once

#include "bogaudio.hpp"
#include "lfo_base.hpp"

using namespace bogaudio::dsp;

extern Model* modelFourFO;

namespace bogaudio {

struct FourFO : LFOBase {
	enum ParamsIds {
		FREQUENCY_PARAM,
		WAVE_PARAM,
		SLOW_PARAM,
		SAMPLE_PWM_PARAM,
		SMOOTH_PARAM,
		OFFSET_PARAM,
		SCALE_PARAM,
		PHASE0_PARAM,
		PHASE1_PARAM,
		PHASE2_PARAM,
		PHASE3_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		SAMPLE_PWM_INPUT,
		SMOOTH_INPUT,
		OFFSET_INPUT,
		SCALE_INPUT,
		PITCH_INPUT,
		RESET_INPUT,
		PHASE0_INPUT,
		PHASE1_INPUT,
		PHASE2_INPUT,
		PHASE3_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		PHASE0_OUTPUT,
		PHASE1_OUTPUT,
		PHASE2_OUTPUT,
		PHASE3_OUTPUT,
		NUM_OUTPUTS
	};

	enum Wave {
		NO_WAVE,
		RAMP_UP_WAVE,
		RAMP_DOWN_WAVE,
		SINE_WAVE,
		TRIANGLE_WAVE,
		SQUARE_WAVE,
		STEPPED_WAVE
	};

	struct Engine {
		int sampleSteps = 1;
		int sampleStep = 0;
		float offset = 0.0f;
		float scale = 0.0f;
		PositiveZeroCrossing resetTrigger;

		Phasor phasor;
		SineTableOscillator sine;
		TriangleOscillator triangle;
		SawOscillator ramp;
		SquareOscillator square;
		SteppedRandomOscillator stepped;

		Phasor::phase_delta_t phase3Offset = 0.0f;
		Phasor::phase_delta_t phase2Offset = 0.0f;
		Phasor::phase_delta_t phase1Offset = 0.0f;
		Phasor::phase_delta_t phase0Offset = 0.0f;

		float phase3Sample = 0.0f;
		float phase2Sample = 0.0f;
		float phase1Sample = 0.0f;
		float phase0Sample = 0.0f;

		bool phase3Active = false;
		bool phase2Active = false;
		bool phase1Active = false;
		bool phase0Active = false;

		Smoother phase3Smoother;
		Smoother phase2Smoother;
		Smoother phase1Smoother;
		Smoother phase0Smoother;

		void reset();
		void sampleRateChange();
	};

	const float amplitude = 5.0f;
	Wave _wave = NO_WAVE;
	Engine* _engines[maxChannels] {};

	FourFO() : LFOBase(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {
		configParam<LFOFrequencyParamQuantity>(FREQUENCY_PARAM, -5.0f, 8.0f, 0.0, "Frequency", " Hz");
		configSwitch(WAVE_PARAM, 1.0, 6.0, 3.0, "Waveform", {"Ramp up", "Ramp down", "Sine", "Triangle", "Square", "Stepped"});
		paramQuantities[WAVE_PARAM]->snapEnabled = true;
		configButton(SLOW_PARAM, "Slow");
		configParam(SAMPLE_PWM_PARAM, -1.0, 1.0, 0.0, "Width", "%", 0.0f, 100.0f);
		configParam(SMOOTH_PARAM, 0.0f, 1.0f, 0.0f, "Smoothing", "%", 0.0f, 100.0f);
		configParam(OFFSET_PARAM, -1.0, 1.0, 0.0, "Offset", " V", 0.0f, 5.0f);
		configParam(SCALE_PARAM, 0.0, 1.0, 1.0, "Scale", "%", 0.0f, 100.0f);
		configParam(PHASE3_PARAM, -1.0, 1.0, 0.0, "Phase 270", "º", 0.0f, 180.0f);
		configParam(PHASE2_PARAM, -1.0, 1.0, 0.0, "Phase 180", "º", 0.0f, 180.0f);
		configParam(PHASE1_PARAM, -1.0, 1.0, 0.0, "Phase 90", "º", 0.0f, 180.0f);
		configParam(PHASE0_PARAM, -1.0, 1.0, 0.0f, "Phase 0", "º", 0.0f, 180.0f);
	}

	void reset() override;
	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
	Phasor::phase_delta_t phaseOffset(int c, Param& p, Input& i, Phasor::phase_delta_t baseOffset);
	void updateOutput(int c, bool useSample, Output& output, Phasor::phase_delta_t& offset, float& sample, bool& active, Smoother& smoother);
};

} // namespace bogaudio
