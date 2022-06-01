#pragma once

#include "bogaudio.hpp"
#include "lfo_base.hpp"

using namespace bogaudio::dsp;

extern Model* modelLFO;

namespace bogaudio {

struct LFO : LFOBase {
	enum ParamsIds {
		FREQUENCY_PARAM,
		SLOW_PARAM,
		SAMPLE_PARAM,
		PW_PARAM,
		OFFSET_PARAM,
		SCALE_PARAM,
		SMOOTH_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		SAMPLE_INPUT,
		PW_INPUT,
		OFFSET_INPUT,
		SCALE_INPUT,
		PITCH_INPUT,
		RESET_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		RAMP_UP_OUTPUT,
		RAMP_DOWN_OUTPUT,
		SQUARE_OUTPUT,
		TRIANGLE_OUTPUT,
		SINE_OUTPUT,
		STEPPED_OUTPUT,
		NUM_OUTPUTS
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

		float sineSample = 0.0f;
		float triangleSample = 0.0f;
		float rampUpSample = 0.0f;
		float rampDownSample = 0.0f;
		float squareSample = 0.0f;
		float steppedSample = 0.0f;

		bool sineActive = false;
		bool triangleActive = false;
		bool rampUpActive = false;
		bool rampDownActive = false;
		bool squareActive = false;
		bool steppedActive = false;

		Smoother sineSmoother;
		Smoother triangleSmoother;
		Smoother rampUpSmoother;
		Smoother rampDownSmoother;
		Smoother squareSmoother;
		Smoother steppedSmoother;

		void reset();
		void sampleRateChange();
	};

	const float amplitude = 5.0f;
	Engine* _engines[maxChannels] {};
	bool _useOffsetCvForSmooth = false;

	LFO() : LFOBase(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {
		configParam<LFOFrequencyParamQuantity>(FREQUENCY_PARAM, -5.0f, 8.0f, 0.0f, "Frequency", " Hz");
		configButton(SLOW_PARAM, "Slow");
		configParam(SAMPLE_PARAM, 0.0f, 1.0f, 0.0f, "Output sampling", "%", 0.0f, 100.0f);
		configParam(PW_PARAM, -1.0f, 1.0f, 0.0f, "Pulse width", "%", 0.0f, 100.0f*0.5f*(1.0f - 2.0f * SquareOscillator::minPulseWidth), 50.0f);
		configParam(SMOOTH_PARAM, 0.0f, 1.0f, 0.0f, "Smoothing", "%", 0.0f, 100.0f);
		configParam<OffsetParamQuantity>(OFFSET_PARAM, -1.0f, 1.0f, 0.0f, "Offset", " V", 0.0f, 5.0f);
		configParam(SCALE_PARAM, 0.0f, 1.0f, 1.0f, "Scale", "%", 0.0f, 100.0f);

		configInput(SAMPLE_INPUT, "Sample CV");
		configInput(PW_INPUT, "Pulse width CV");
		configInput(OFFSET_INPUT, "Offset CV");
		configInput(SCALE_INPUT, "Scale CV");
		configInput(PITCH_INPUT, "Pitch (1V/octave)");
		configInput(RESET_INPUT, "Reset");

		configOutput(RAMP_UP_OUTPUT, "Ramp up");
		configOutput(RAMP_DOWN_OUTPUT, "Ramp down");
		configOutput(SQUARE_OUTPUT, "Square");
		configOutput(TRIANGLE_OUTPUT, "Triangle");
		configOutput(SINE_OUTPUT, "Sine");
		configOutput(STEPPED_OUTPUT, "Stepped");
	}

	void reset() override;
	void sampleRateChange() override;
	json_t* saveToJson(json_t* root) override;
	void loadFromJson(json_t* root) override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void updateOutput(int c, Phasor& wave, bool useSample, bool invert, Output& output, float& sample, bool& active, Smoother& smoother);
};

} // namespace bogaudio
