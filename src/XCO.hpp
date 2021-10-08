#pragma once

#include "bogaudio.hpp"
#include "dsp/filters/resample.hpp"
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

	struct Engine {
		static constexpr int oversample = 8;

		float frequency = 0.0f;
		float baseVOct = 0.0f;
		float baseHz = 0.0f;
		float fmDepth = 0.0f;
		float triangleSampleWidth = 0.0f;
		float sineFeedback = 0.0f;
		float sineOMix = 0.0f;
		float sineFeedbackDelayedSample = 0.0f;
		Phasor::phase_delta_t squarePhaseOffset = 0.0f;
		Phasor::phase_delta_t sawPhaseOffset = 0.0f;
		Phasor::phase_delta_t trianglePhaseOffset = 0.0f;
		Phasor::phase_delta_t sinePhaseOffset = 0.0f;
		float squareMix = 1.0f;
		float sawMix = 1.0f;
		float triangleMix = 1.0f;
		float sineMix = 1.0f;

		Phasor phasor;
		BandLimitedSquareOscillator square;
		BandLimitedSawOscillator saw;
		TriangleOscillator triangle;
		SineTableOscillator sine;
		CICDecimator squareDecimator;
		CICDecimator sawDecimator;
		CICDecimator triangleDecimator;
		CICDecimator sineDecimator;
		float squareBuffer[oversample];
		float sawBuffer[oversample];
		float triangleBuffer[oversample];
		float sineBuffer[oversample];
		PositiveZeroCrossing syncTrigger;

		bogaudio::dsp::SlewLimiter fmDepthSL;
		bogaudio::dsp::SlewLimiter squarePulseWidthSL;
		bogaudio::dsp::SlewLimiter sawSaturationSL;
		bogaudio::dsp::SlewLimiter triangleSampleWidthSL;
		bogaudio::dsp::SlewLimiter sineFeedbackSL;
		bogaudio::dsp::SlewLimiter squareMixSL;
		bogaudio::dsp::SlewLimiter sawMixSL;
		bogaudio::dsp::SlewLimiter triangleMixSL;
		bogaudio::dsp::SlewLimiter sineMixSL;

		Engine() {
			saw.setQuality(12);
			square.setQuality(12);
		}

		void reset();
		void sampleRateChange(float sampleRate);
		void setFrequency(float frequency);
	};

	const float amplitude = 5.0f;
	const float _slowModeOffset = -7.0f;
	const float sineOversampleMixIncrement = 0.01f;
	float _oversampleThreshold = 0.0f;
	bool _slowMode = false;
	bool _fmLinearMode = false;
	bool _dcCorrection = true;
	Engine* _engines[maxChannels] {};

	struct XCOFrequencyParamQuantity : FrequencyParamQuantity {
		float offset() override;
	};

	XCO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam<XCOFrequencyParamQuantity>(FREQUENCY_PARAM, -3.0f, 6.0f, 0.0f, "Frequency", " Hz");
		configParam(FINE_PARAM, -1.0f, 1.0f, 0.0f, "Fine tune", " cents", 0.0f, 100.0f);
		configButton(SLOW_PARAM, "Slow mode");
		configParam(FM_DEPTH_PARAM, 0.0f, 1.0f, 0.0f, "FM depth", "%", 0.0f, 100.0f);
		configSwitch(FM_TYPE_PARAM, 0.0f, 1.0f, 1.0f, "FM mode", {"Linear FM", "Exponential FM"});
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

		configInput(FM_INPUT, "FM");
		configInput(FM_DEPTH_INPUT, "FM_DEPTH");
		configInput(SQUARE_PW_INPUT, "SQUARE_PW");
		configInput(SQUARE_PHASE_INPUT, "SQUARE_PHASE");
		configInput(SQUARE_MIX_INPUT, "SQUARE_MIX");
		configInput(SAW_SATURATION_INPUT, "SAW_SATURATION");
		configInput(SAW_PHASE_INPUT, "SAW_PHASE");
		configInput(SAW_MIX_INPUT, "SAW_MIX");
		configInput(TRIANGLE_SAMPLE_INPUT, "TRIANGLE_SAMPLE");
		configInput(TRIANGLE_PHASE_INPUT, "TRIANGLE_PHASE");
		configInput(TRIANGLE_MIX_INPUT, "TRIANGLE_MIX");
		configInput(SINE_FEEDBACK_INPUT, "SINE_FEEDBACK");
		configInput(SINE_PHASE_INPUT, "SINE_PHASE");
		configInput(SINE_MIX_INPUT, "SINE_MIX");
		configInput(PITCH_INPUT, "PITCH");
		configInput(SYNC_INPUT, "SYNC");

		configOutput(SQUARE_OUTPUT, "SQUARE");
		configOutput(SAW_OUTPUT, "SAW");
		configOutput(TRIANGLE_OUTPUT, "TRIANGLE");
		configOutput(SINE_OUTPUT, "SINE");
		configOutput(MIX_OUTPUT, "MIX");
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
	Phasor::phase_delta_t phaseOffset(int c, Param& param, Input& input);
	float level(int c, Param& param, Input& input);
};

} // namespace bogaudio
