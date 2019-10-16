#pragma once

#include "bogaudio.hpp"
#include "dsp/filter.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelVCO;

namespace bogaudio {

struct VCO : BGModule {
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

	enum LightsIds {
		SLOW_LIGHT,
		LINEAR_LIGHT,
		NUM_LIGHTS
	};

	struct Engine {
		static constexpr int oversample = 8;

		float frequency = NAN;
		float baseVOct = 0.0f;
		float baseHz = 0.0f;

		Phasor phasor;
		BandLimitedSquareOscillator square;
		
		BandLimitedSawOscillator saw;
		TriangleOscillator triangle;
		SineTableOscillator sine;
		CICDecimator squareDecimator;
		CICDecimator sawDecimator;
		CICDecimator triangleDecimator;
		float squareBuffer[oversample];
		float sawBuffer[oversample];
		float triangleBuffer[oversample];
		PositiveZeroCrossing syncTrigger;
		bogaudio::dsp::SlewLimiter squarePulseWidthSL;

		Engine() {
			saw.setQuality(12);
			square.setQuality(12);
		}
		void reset();
		void sampleRateChange(float sampleRate);
		void setFrequency(float frequency);
	};

	const float amplitude = 5.0f;
	const float slowModeOffset = -7.0f;
	Engine* _engines[maxChannels] {};
	float _oversampleThreshold = 0.0f;
	bool _slowMode = false;
	bool _linearMode = false;
	float _fmDepth = 0.0f;
	bool _fmLinearMode = false;

	struct VCOFrequencyParamQuantity : FrequencyParamQuantity {
		float offset() override;
		float getDisplayValue() override;
		void setDisplayValue(float v) override;
	};

	VCO() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<VCOFrequencyParamQuantity>(FREQUENCY_PARAM, -3.0f, 6.0f, 0.0f, "Frequency", " Hz");
		configParam(FINE_PARAM, -1.0f, 1.0f, 0.0f, "Fine tune", " cents", 0.0f, 100.0f);
		configParam(SLOW_PARAM, 0.0f, 1.0f, 0.0f, "Slow mode");
		configParam(PW_PARAM, -1.0f, 1.0f, 0.0f, "Pulse width", "%", 0.0f, 100.0f*0.5f*(1.0f - 2.0f * SquareOscillator::minPulseWidth), 50.0f);
		configParam(FM_PARAM, 0.0f, 1.0f, 0.0f, "FM depth", "%", 0.0f, 100.0f);
		configParam(FM_TYPE_PARAM, 0.0f, 1.0f, 1.0f, "FM mode");
		configParam(LINEAR_PARAM, 0.0f, 1.0f, 0.0f, "Linear Freq");
	}

	void reset() override;
	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addEngine(int c) override;
	void removeEngine(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void always(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
