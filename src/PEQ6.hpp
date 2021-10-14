#pragma once

#include "PEQ6_shared.hpp"

namespace bogaudio {

struct PEQ6 : ExpandableModule<PEQ6ExpanderMessage, BandExcludeModule> {
	enum ParamsIds {
		FREQUENCY_CV_PARAM,
		BANDWIDTH_PARAM,
		LP_PARAM,
		HP_PARAM,
		FMOD_PARAM,
		LEVEL1_PARAM,
		FREQUENCY1_PARAM,
		FREQUENCY_CV1_PARAM,
		LEVEL2_PARAM,
		FREQUENCY2_PARAM,
		FREQUENCY_CV2_PARAM,
		LEVEL3_PARAM,
		FREQUENCY3_PARAM,
		FREQUENCY_CV3_PARAM,
		LEVEL4_PARAM,
		FREQUENCY4_PARAM,
		FREQUENCY_CV4_PARAM,
		LEVEL5_PARAM,
		FREQUENCY5_PARAM,
		FREQUENCY_CV5_PARAM,
		LEVEL6_PARAM,
		FREQUENCY6_PARAM,
		FREQUENCY_CV6_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		FREQUENCY_CV_INPUT,
		BANDWIDTH_INPUT,
		IN_INPUT,
		LEVEL1_INPUT,
		FREQUENCY_CV1_INPUT,
		LEVEL2_INPUT,
		FREQUENCY_CV2_INPUT,
		LEVEL3_INPUT,
		FREQUENCY_CV3_INPUT,
		LEVEL4_INPUT,
		FREQUENCY_CV4_INPUT,
		LEVEL5_INPUT,
		FREQUENCY_CV5_INPUT,
		LEVEL6_INPUT,
		FREQUENCY_CV6_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUT4_OUTPUT,
		OUT5_OUTPUT,
		OUT6_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		FMOD_RELATIVE_LIGHT,
		FMOD_FULL_LIGHT,
		NUM_LIGHTS
	};

	PEQEngine* _engines[maxChannels] {};
	float _rmsSums[6] {};
	float _rms[6] {};
	bool _fullFrequencyMode = false;
	PEQ6ExpanderMessage* _expanderMessage = NULL;

	PEQ6() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		float levelDefault = fabsf(PEQChannel::minDecibels) / (PEQChannel::maxDecibels - PEQChannel::minDecibels);
		configParam(FREQUENCY_CV_PARAM, -1.0f, 1.0f, 0.0f, "Global frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(BANDWIDTH_PARAM, 0.0f, 1.0f, 0.33f, "Bandwidth", "%", 0.0f, 100.0f);
		configSwitch(LP_PARAM, 0.0f, 1.0f, 1.0f, "Channel 1 LP/BP", {"Bandpass", "Lowpass"});
		configSwitch(HP_PARAM, 0.0f, 1.0f, 1.0f, "Channel 6 HP/BP", {"Bandpass", "Highpass"});
		configSwitch(FMOD_PARAM, 0.0f, 1.0f, 0.0f, "Frequency modulation range", {"Octave", "Full"});
		configParam(LEVEL1_PARAM, 0.0f, 1.0f, levelDefault, "Channel 1 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY1_PARAM, 0.0f, 1.0f, 0.0707107f, "Channel 1 frequency", " HZ");
		configParam(FREQUENCY_CV1_PARAM, -1.0f, 1.0f, 1.0f, "Channel 1 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL2_PARAM, 0.0f, 1.0f, levelDefault, "Channel 2 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY2_PARAM, 0.0f, 1.0f, 0.0935414f, "Channel 2 frequency", " HZ");
		configParam(FREQUENCY_CV2_PARAM, -1.0f, 1.0f, 1.0f, "Channel 2 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL3_PARAM, 0.0f, 1.0f, levelDefault, "Channel 3 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY3_PARAM, 0.0f, 1.0f, 0.1322876f, "Channel 3 frequency", " HZ");
		configParam(FREQUENCY_CV3_PARAM, -1.0f, 1.0f, 1.0f, "Channel 3 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL4_PARAM, 0.0f, 1.0f, levelDefault, "Channel 4 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY4_PARAM, 0.0f, 1.0f, 0.1870829f, "Channel 4 frequency", " HZ");
		configParam(FREQUENCY_CV4_PARAM, -1.0f, 1.0f, 1.0f, "Channel 4 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL5_PARAM, 0.0f, 1.0f, levelDefault, "Channel 5 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY5_PARAM, 0.0f, 1.0f, 0.2645751f, "Channel 5 frequency", " HZ");
		configParam(FREQUENCY_CV5_PARAM, -1.0f, 1.0f, 1.0f, "Channel 5 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL6_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY6_PARAM, 0.0f, 1.0f, 0.3535534f, "Channel 6 frequency", " HZ");
		configParam(FREQUENCY_CV6_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 frequency CV attenuation", "%", 0.0f, 100.0f);
		configBypass(IN_INPUT, OUT_OUTPUT);

		configInput(FREQUENCY_CV_INPUT, "Frequency CV");
		configInput(BANDWIDTH_INPUT, "Bandwidth CV");
		configInput(IN_INPUT, "Signal");
		configInput(LEVEL1_INPUT, "Channel 1 level CV");
		configInput(FREQUENCY_CV1_INPUT, "Channel 1 frequency CV");
		configInput(LEVEL2_INPUT, "Channel 2 level CV");
		configInput(FREQUENCY_CV2_INPUT, "Channel 2 frequency CV");
		configInput(LEVEL3_INPUT, "Channel 3 level CV");
		configInput(FREQUENCY_CV3_INPUT, "Channel 3 frequency CV");
		configInput(LEVEL4_INPUT, "Channel 4 level CV");
		configInput(FREQUENCY_CV4_INPUT, "Channel 4 frequency CV");
		configInput(LEVEL5_INPUT, "Channel 5 level CV");
		configInput(FREQUENCY_CV5_INPUT, "Channel 5 frequency CV");
		configInput(LEVEL6_INPUT, "Channel 6 level CV");
		configInput(FREQUENCY_CV6_INPUT, "Channel 6 frequency CV");

		configOutput(OUT_OUTPUT, "All channels mix");
		configOutput(OUT1_OUTPUT, "Channel 1");
		configOutput(OUT2_OUTPUT, "Channel 2");
		configOutput(OUT3_OUTPUT, "Channel 3");
		configOutput(OUT4_OUTPUT, "Channel 4");
		configOutput(OUT5_OUTPUT, "Channel 5");
		configOutput(OUT6_OUTPUT, "Channel 6");

		setExpanderModelPredicate([](Model* m) { return m == modelPEQ6XF; });
	}

	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
	void postProcessAlways(const ProcessArgs& args) override;
};

} // namespace bogaudio
