#pragma once

#include "bogaudio.hpp"
#include "dsp/oscillator.hpp"

using namespace bogaudio::dsp;

extern Model* modelXCO;

namespace bogaudio {

struct XCO : Module {
	enum ParamsIds {
		FREQUENCY_PARAM,
		FINE_PARAM,
		SLOW_PARAM,
		FM_PARAM,
		FM_TYPE_PARAM,
		SQUARE_PW_PARAM,
		SQUARE_PHASE_PARAM,
		SQUARE_MIX_PARAM,
		SAW_SATURATION_PARAM,
		SAW_PHASE_PARAM,
		SAW_MIX_PARAM,
		TRIANGLE_SAMPLE_PARAM,
		TRIANGLE_PHASE_PARAM,
		TRIANGLE_MIX_PARAM,
		SINE_FEEDBACK_PARAM,
		SINE_PHASE_PARAM,
		SINE_MIX_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		FM_INPUT,
		SQUARE_PW_INPUT,
		SQUARE_PHASE_INPUT,
		SQUARE_MIX_INPUT,
		SAW_SATURATION_INPUT,
		SAW_PHASE_INPUT,
		SAW_MIX_INPUT,
		TRIANGLE_SAMPLE_INPUT,
		TRIANGLE_PHASE_INPUT,
		TRIANGLE_MIX_INPUT,
		SINE_FEEDBACK_INPUT,
		SINE_PHASE_INPUT,
		SINE_MIX_INPUT,
		PITCH_INPUT,
		SYNC_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		SQUARE_OUTPUT,
		SAW_OUTPUT,
		TRIANGLE_OUTPUT,
		SINE_OUTPUT,
		MIX_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		SLOW_LIGHT,
		NUM_LIGHTS
	};

	const int modulationSteps = 100;
	const float amplitude = 5.0f;
	int _modulationStep = 0;
	float _baseVOct = 0.0f;
	float _baseHz = 0.0f;
	bool _slowMode = false;
	float _fmDepth = 0.0f;
	bool _fmLinearMode = false;
	float _sineFeedback = 0.0f;
	float _squarePhaseOffset = 0.0f;
	float _sawPhaseOffset = 0.0f;
	float _trianglePhaseOffset = 0.0f;
	float _sinePhaseOffset = 0.0f;
	float _squareMix = 1.0f;
	float _sawMix = 1.0f;
	float _triangleMix = 1.0f;
	float _sineMix = 1.0f;
	SchmittTrigger _syncTrigger;

	Phasor _phasor;
	BandLimitedSquareOscillator _square;
	BandLimitedSawOscillator _saw;
	TriangleOscillator _triangle;
	SineTableOscillator _sine;

	XCO()
	: Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS)
	, _phasor(engineGetSampleRate(), 1.0f)
	, _square(0.0f, 0.0f)
	, _saw(0.0f, 0.0f)
	, _triangle(0.0f, 0.0f)
	, _sine(0.0f, 0.0f)
	{
		onReset();
		_saw.setQuality(12);
		_square.setQuality(12);
	}

	virtual void onReset() override;
	virtual void onSampleRateChange() override;
	virtual void step() override;
	float phaseOffset(Param& param, Input& input);
	float level(Param& param, Input& input);
};

} // namespace bogaudio
