#pragma once

#include "bogaudio.hpp"
#include "dsp/filter.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelXCO;

namespace bogaudio {

struct XCO : BGModule {
	enum ParamsIds {
		FREQUENCY_PARAM,
		FINE_PARAM,
		SLOW_PARAM,
		FM_DEPTH_PARAM,
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
		FM_DEPTH_INPUT,
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

	const float amplitude = 5.0f;
	static constexpr int oversample = 8;
	const float _slowModeOffset = -7.0f;
	const float sineOversampleMixIncrement = 0.01f;
	float _oversampleThreshold = 0.0f;
	float _frequency = 0.0f;
	float _baseVOct = 0.0f;
	float _baseHz = 0.0f;
	bool _slowMode = false;
	float _fmDepth = 0.0f;
	bool _fmLinearMode = false;
	float _triangleSampleWidth = 0.0f;
	float _sineFeedback = 0.0f;
	float _sineOMix = 0.0f;
	float _sineFeedbackDelayedSample = 0.0f;
	Phasor::phase_delta_t _squarePhaseOffset = 0.0f;
	Phasor::phase_delta_t _sawPhaseOffset = 0.0f;
	Phasor::phase_delta_t _trianglePhaseOffset = 0.0f;
	Phasor::phase_delta_t _sinePhaseOffset = 0.0f;
	float _squareMix = 1.0f;
	float _sawMix = 1.0f;
	float _triangleMix = 1.0f;
	float _sineMix = 1.0f;

	Phasor _phasor;
	BandLimitedSquareOscillator _square;
	BandLimitedSawOscillator _saw;
	TriangleOscillator _triangle;
	SineTableOscillator _sine;
	CICDecimator _squareDecimator;
	CICDecimator _sawDecimator;
	CICDecimator _triangleDecimator;
	CICDecimator _sineDecimator;
	float _squareBuffer[oversample];
	float _sawBuffer[oversample];
	float _triangleBuffer[oversample];
	float _sineBuffer[oversample];
	PositiveZeroCrossing _syncTrigger;

	bogaudio::dsp::SlewLimiter _fmDepthSL;
	bogaudio::dsp::SlewLimiter _squarePulseWidthSL;
	bogaudio::dsp::SlewLimiter _sawSaturationSL;
	bogaudio::dsp::SlewLimiter _triangleSampleWidthSL;
	bogaudio::dsp::SlewLimiter _sineFeedbackSL;
	bogaudio::dsp::SlewLimiter _squareMixSL;
	bogaudio::dsp::SlewLimiter _sawMixSL;
	bogaudio::dsp::SlewLimiter _triangleMixSL;
	bogaudio::dsp::SlewLimiter _sineMixSL;

	struct XCOFrequencyParamQuantity : FrequencyParamQuantity {
		float offset() override;
	};

	XCO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<XCOFrequencyParamQuantity>(FREQUENCY_PARAM, -3.0f, 6.0f, 0.0f, "Frequency", " Hz");
		configParam(FINE_PARAM, -1.0f, 1.0f, 0.0f, "Fine tune", " cents", 0.0f, 100.0f);
		configParam(SLOW_PARAM, 0.0f, 1.0f, 0.0f, "Slow mode");
		configParam(FM_DEPTH_PARAM, 0.0f, 1.0f, 0.0f, "FM depth", "%", 0.0f, 100.0f);
		configParam(FM_TYPE_PARAM, 0.0f, 1.0f, 1.0f, "FM mode");
		configParam(SQUARE_PW_PARAM, -0.97f, 0.97f, 0.0f, "Square wave pulse width", "%", 0.0f, 100.0f);
		configParam(SQUARE_PHASE_PARAM, -1.0f, 1.0f, 0.0f, "Square wave phase", "º", 0.0f, 180.0f);
		configParam(SQUARE_MIX_PARAM, 0.0f, 1.0f, 1.0f, "Square wave mix", "%", 0.0f, 100.0f);
		configParam(SAW_SATURATION_PARAM, 0.0f, 1.0f, 0.0f, "Saw wave saturation", "%", 0.0f, 100.0f);
		configParam(SAW_PHASE_PARAM, -1.0f, 1.0f, 0.0f, "Saw wave phase", "º", 0.0f, 180.0f);
		configParam(SAW_MIX_PARAM, 0.0f, 1.0f, 1.0f, "Saw wave mix", "%", 0.0f, 100.0f);
		configParam(TRIANGLE_SAMPLE_PARAM, 0.0f, 1.0f, 0.0f, "Triangle wave sampling", "%", 0.0f, 100.0f);
		configParam(TRIANGLE_PHASE_PARAM, -1.0f, 1.0f, 0.0f, "Triangle wave phase", "º", 0.0f, 180.0f);
		configParam(TRIANGLE_MIX_PARAM, 0.0f, 1.0f, 1.0f, "Triangle wave mix", "%", 0.0f, 100.0f);
		configParam(SINE_FEEDBACK_PARAM, 0.0f, 1.0f, 0.0f, "Sine wave feedback", "%", 0.0f, 100.0f);
		configParam(SINE_PHASE_PARAM, -1.0f, 1.0f, 0.0f, "Sine wave phase", "º", 0.0f, 180.0f);
		configParam(SINE_MIX_PARAM, 0.0f, 1.0f, 1.0f, "Sine wave mix", "%", 0.0f, 100.0f);

		reset();
		setSampleRate(APP->engine->getSampleRate());
		_saw.setQuality(12);
		_square.setQuality(12);
	}

	void reset() override;
	void sampleRateChange() override;
	bool active() override;
	void modulate() override;
	void always(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int _c) override;
	Phasor::phase_delta_t phaseOffset(Param& param, Input& input);
	float level(Param& param, Input& input);
	void setSampleRate(float sampleRate);
	void setFrequency(float frequency);
};

} // namespace bogaudio
