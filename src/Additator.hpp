#pragma once

#include "bogaudio.hpp"
#include "dsp/oscillator.hpp"
#include "dsp/pitch.hpp"
#include "dsp/signal.hpp"

using namespace bogaudio::dsp;

extern Model* modelAdditator;

namespace bogaudio {

struct Additator : BGModule {
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

	static constexpr int maxPartials = 100;
	static constexpr float maxWidth = 2.0f;
	static constexpr float maxSkew = 0.99f;
	static constexpr float minAmplitudeNormalization = 1.0f;
	static constexpr float maxAmplitudeNormalization = 5.0f;
	static constexpr float minDecay = -1.0f;
	static constexpr float maxDecay = 3.0f;
	static constexpr float minFilter = 0.1;
	static constexpr float maxFilter = 1.9;
	static constexpr float slewLimitTime = 1.0f;

	struct Engine {
		int partials = 0;
		float width = 0.0f;
		float oddSkew = 0.0f;
		float evenSkew = 0.0f;
		float amplitudeNormalization = 0.0f;
		float decay = 0.0f;
		float balance = 0.0f;
		float filter = 0.0f;
		Phase phase = PHASE_RESET;
		float maxFrequency = 0.0f;
		int activePartials = 1;
		SineBankOscillator oscillator;
		PositiveZeroCrossing syncTrigger;
		bogaudio::dsp::SlewLimiter widthSL;
		bogaudio::dsp::SlewLimiter oddSkewSL;
		bogaudio::dsp::SlewLimiter evenSkewSL;
		bogaudio::dsp::SlewLimiter amplitudeNormalizationSL;
		bogaudio::dsp::SlewLimiter decaySL;
		bogaudio::dsp::SlewLimiter balanceSL;
		bogaudio::dsp::SlewLimiter filterSL;

		Engine() : oscillator(1000.0f, 100.0f, maxPartials) {}

		void reset();
		void sampleRateChange();
	};

	Engine* _engines[maxChannels] {};

	Additator()	{
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<FrequencyParamQuantity>(FREQUENCY_PARAM, -3.0f, 6.0f, 0.0f, "Frequency", " Hz");
		configParam(PARTIALS_PARAM, 1.0f, Additator::maxPartials, Additator::maxPartials / 5.0f, "Partials");
		paramQuantities[PARTIALS_PARAM]->snapEnabled = true;
		configParam(FINE_PARAM, -1.0f, 1.0f, 0.0f, "Fine tune", " cents", 0.0f, 100.0f);
		configParam(WIDTH_PARAM, 0.0f, maxWidth, maxWidth / 2.0f, "Width", "%", 0.0f, 2.0f * (1.0f / maxWidth) * 100.0f, -100.0f);
		configParam(ODD_SKEW_PARAM, -maxSkew, maxSkew, 0.0f, "Odd skew", "%", 0.0f, (1.0f / maxSkew) * 100.0f);
		configParam(EVEN_SKEW_PARAM, -maxSkew, maxSkew, 0.0f, "Even skew", "%", 0.0f, (1.0f / maxSkew) * 100.0f);
		configParam(GAIN_PARAM, minAmplitudeNormalization, maxAmplitudeNormalization, (maxAmplitudeNormalization - minAmplitudeNormalization) / 2.0 + minAmplitudeNormalization, "Gain");
		configParam(DECAY_PARAM, minDecay, maxDecay, (maxDecay - minDecay) / 2.0 + minDecay, "Decay");
		configParam(BALANCE_PARAM, -1.0f, 1.0f, 0.0f, "Balance", "%", 0.0f, 100.0f);
		configParam(FILTER_PARAM, minFilter, maxFilter, (maxFilter - minFilter) / 2.0 + minFilter, "Filter");
		configSwitch(PHASE_PARAM, 1.0f, 2.0f, 1.0f, "Phase", {"Sine", "Cosine"});

		configInput(PITCH_INPUT, "Pitch (1V/octave)");
		configInput(SYNC_INPUT, "Sync");
		configInput(PARTIALS_INPUT, "Partials CV");
		configInput(WIDTH_INPUT, "Width CV");
		configInput(ODD_SKEW_INPUT, "Odd skew CV");
		configInput(EVEN_SKEW_INPUT, "Even skew CV");
		configInput(GAIN_INPUT, "Gain CV");
		configInput(DECAY_INPUT, "Decay CV");
		configInput(BALANCE_INPUT, "Balance CV");
		configInput(FILTER_INPUT, "Filter CV");

		configOutput(AUDIO_OUTPUT, "Signal");
	}

	void reset() override;
	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	float widthParam(int c);
	float oddSkewParam(int c);
	float evenSkewParam(int c);
	float amplitudeNormalizationParam(int c);
	float decayParam(int c);
	float balanceParam(int c);
	float filterParam(int c);
	void modulateChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	float cvValue(int c, Input& cv, bool dc = false);
};

} // namespace bogaudio
