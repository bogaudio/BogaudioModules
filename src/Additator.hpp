#pragma once

#include "bogaudio.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/pitch.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelAdditator;

namespace bogaudio {

struct Additator : ModulatingBGModule {
	enum ParamsIds {
		FREQUENCY_PARAM,
		PARTIALS_PARAM,
		WIDTH_PARAM,
		ODD_SKEW_PARAM,
		EVEN_SKEW_PARAM,
		GAIN_PARAM,
		DECAY_PARAM,
		BALANCE_PARAM,
		FILTER_PARAM,
		PHASE_PARAM,
		FINE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		PITCH_INPUT,
		SYNC_INPUT,
		PARTIALS_INPUT,
		WIDTH_INPUT,
		ODD_SKEW_INPUT,
		EVEN_SKEW_INPUT,
		GAIN_INPUT,
		DECAY_INPUT,
		BALANCE_INPUT,
		FILTER_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		AUDIO_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		SINE_LIGHT,
		COSINE_LIGHT,
		NUM_LIGHTS
	};

	enum Phase {
		PHASE_RESET,
		PHASE_SINE,
		PHASE_COSINE
	};

	const int maxPartials = 100;
	const float maxWidth = 2.0f;
	const float maxSkew = 0.99f;
	const float minAmplitudeNormalization = 1.0f;
	const float maxAmplitudeNormalization = 5.0f;
	const float minDecay = -1.0f;
	const float maxDecay = 3.0f;
	const float minFilter = 0.1;
	const float maxFilter = 1.9;
	const float slewLimitTime = 1.0f;

	int _partials = 0;
	float _width = 0.0f;
	float _oddSkew = 0.0f;
	float _evenSkew = 0.0f;
	float _amplitudeNormalization = 0.0f;
	float _decay = 0.0f;
	float _balance = 0.0f;
	float _filter = 0.0f;
	Phase _phase = PHASE_RESET;
	float _maxFrequency = 0.0f;
	int _activePartials = 1;
	SineBankOscillator _oscillator;
	PositiveZeroCrossing _syncTrigger;
	bogaudio::dsp::SlewLimiter _widthSL;
	bogaudio::dsp::SlewLimiter _oddSkewSL;
	bogaudio::dsp::SlewLimiter _evenSkewSL;
	bogaudio::dsp::SlewLimiter _amplitudeNormalizationSL;
	bogaudio::dsp::SlewLimiter _decaySL;
	bogaudio::dsp::SlewLimiter _balanceSL;
	bogaudio::dsp::SlewLimiter _filterSL;

	Additator()
	:  _oscillator(1000.0f, 100.0f, maxPartials)
	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<FrequencyParamQuantity>(FREQUENCY_PARAM, -3.0f, 6.0f, 0.0f, "Frequency", " Hz");
		configParam(PARTIALS_PARAM, 1.0f, Additator::maxPartials, Additator::maxPartials / 5.0f, "Partials");
		configParam(FINE_PARAM, -1.0f, 1.0f, 0.0f, "Fine tune", " cents", 0.0f, 100.0f);
		configParam(WIDTH_PARAM, 0.0f, maxWidth, maxWidth / 2.0f, "Width", "%", 0.0f, 2.0f * (1.0f / maxWidth) * 100.0f, -100.0f);
		configParam(ODD_SKEW_PARAM, -maxSkew, maxSkew, 0.0f, "Odd skew", "%", 0.0f, (1.0f / maxSkew) * 100.0f);
		configParam(EVEN_SKEW_PARAM, -maxSkew, maxSkew, 0.0f, "Even skew", "%", 0.0f, (1.0f / maxSkew) * 100.0f);
		configParam(GAIN_PARAM, minAmplitudeNormalization, maxAmplitudeNormalization, (maxAmplitudeNormalization - minAmplitudeNormalization) / 2.0 + minAmplitudeNormalization, "Gain");
		configParam(DECAY_PARAM, minDecay, maxDecay, (maxDecay - minDecay) / 2.0 + minDecay, "Decay");
		configParam(BALANCE_PARAM, -1.0f, 1.0f, 0.0f, "Balance", "%", 0.0f, 100.0f);
		configParam(FILTER_PARAM, minFilter, maxFilter, (maxFilter - minFilter) / 2.0 + minFilter, "Filter");
		configParam(PHASE_PARAM, 1.0f, 2.0f, 1.0f, "Phase");

		reset();
		sampleRateChange();
	}

	void reset() override;
	void sampleRateChange() override;
	bool active() override;
	void modulate() override;
	float cvValue(Input& cv, bool dc = false);
	void alwaysProcess(const ProcessArgs& args) override;
	void processIfActive(const ProcessArgs& args) override;
};

} // namespace bogaudio
