#pragma once

#include "bogaudio.hpp"
#include "dsp/filter.hpp"
#include "dsp/oscillator.hpp"

using namespace bogaudio::dsp;

extern Model* modelVCO;

namespace bogaudio {

struct VCO : Module {
	enum ParamsIds {
		FREQUENCY_PARAM,
		FINE_PARAM,
		SLOW_PARAM,
		PW_PARAM,
		FM_PARAM,
		FM_TYPE_PARAM,
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

	enum LightsIds {
		SLOW_LIGHT,
		NUM_LIGHTS
	};

	const int modulationSteps = 100;
	const float amplitude = 5.0f;
	static constexpr int oversample = 8;
	int _modulationStep = 0;
	float _oversampleThreshold = 0.0f;
	float _frequency = 0.0f;
	float _baseVOct = 0.0f;
	float _baseHz = 0.0f;
	bool _slowMode = false;
	float _fmDepth = 0.0f;
	bool _fmLinearMode = false;
	SchmittTrigger _syncTrigger;

	Phasor _phasor;
	BandLimitedSquareOscillator _square;
	BandLimitedSawOscillator _saw;
	TriangleOscillator _triangle;
	SineTableOscillator _sine;
	Decimator _squareDecimator;
	Decimator _sawDecimator;
	Decimator _triangleDecimator;
	float _squareBuffer[oversample];
	float _sawBuffer[oversample];
	float _triangleBuffer[oversample];

	VCO()
	: Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	, _phasor(0.0f, 1.0f)
	, _square(0.0f, 0.0f)
	, _saw(0.0f, 0.0f)
	, _triangle(0.0f, 0.0f)
	, _sine(0.0f, 0.0f)
	{
		onReset();
		setSampleRate(engineGetSampleRate());
		_saw.setQuality(12);
		_square.setQuality(12);
	}

	virtual void onReset() override;
	virtual void onSampleRateChange() override;
	virtual void step() override;
	void setSampleRate(float sampleRate);
	void setFrequency(float frequency);
};

} // namespace bogaudio
