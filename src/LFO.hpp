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
		NUM_OUTPUTS
	};

	enum LightsIds {
		SLOW_LIGHT,
		NUM_LIGHTS
	};

	const int modulationSteps = 100;
	const float amplitude = 5.0f;

	int _modulationStep = 0;
	int _sampleSteps = 1;
	int _sampleStep = 0;
	float _offset = 0.0f;
	float _scale = 0.0f;
	PositiveZeroCrossing _resetTrigger;

	Phasor _phasor;
	SineTableOscillator _sine;
	TriangleOscillator _triangle;
	SawOscillator _ramp;
	SquareOscillator _square;

	float _sineSample = 0.0f;
	float _triangleSample = 0.0f;
	float _rampUpSample = 0.0f;
	float _rampDownSample = 0.0f;
	float _squareSample = 0.0f;

	bool _sineActive = false;
	bool _triangleActive = false;
	bool _rampUpActive = false;
	bool _rampDownActive = false;
	bool _squareActive = false;

	LFO() : LFOBase(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {
		configParam<LFOFrequencyParamQuantity>(FREQUENCY_PARAM, -5.0f, 8.0f, 0.0f, "Frequency", " Hz");
		configParam(SLOW_PARAM, 0.0f, 1.0f, 0.0f, "Slow");
		configParam(SAMPLE_PARAM, 0.0f, 1.0f, 0.0f, "Output sampling", "%", 0.0f, 100.0f);
		configParam(PW_PARAM, -1.0f, 1.0f, 0.0f, "Pulse width", "%", 0.0f, 100.0f*0.5f*(1.0f - 2.0f * _square.minPulseWidth), 50.0f);
		configParam(OFFSET_PARAM, -1.0f, 1.0f, 0.0f, "Offset", "V", 0.0f, 5.0f);
		configParam(SCALE_PARAM, 0.0f, 1.0f, 1.0f, "Scale", "%", 0.0f, 100.0f);

		onReset();
		onSampleRateChange();
	}

	void onReset() override;
	void onSampleRateChange() override;
	void process(const ProcessArgs& args) override;
	void updateOutput(Phasor& wave, bool useSample, bool invert, Output& output, float& sample, bool& active);
};

} // namespace bogaudio
