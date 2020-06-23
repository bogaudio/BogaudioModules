#pragma once

#include "bogaudio.hpp"
#include "parametric_eq.hpp"

extern Model* modelPEQ14;

namespace bogaudio {

struct PEQ14 : BGModule {
	enum ParamsIds {
		EF_RESPONSE_PARAM,
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
		LEVEL7_PARAM,
		FREQUENCY7_PARAM,
		FREQUENCY_CV7_PARAM,
		LEVEL8_PARAM,
		FREQUENCY8_PARAM,
		FREQUENCY_CV8_PARAM,
		LEVEL9_PARAM,
		FREQUENCY9_PARAM,
		FREQUENCY_CV9_PARAM,
		LEVEL10_PARAM,
		FREQUENCY10_PARAM,
		FREQUENCY_CV10_PARAM,
		LEVEL11_PARAM,
		FREQUENCY11_PARAM,
		FREQUENCY_CV11_PARAM,
		LEVEL12_PARAM,
		FREQUENCY12_PARAM,
		FREQUENCY_CV12_PARAM,
		LEVEL13_PARAM,
		FREQUENCY13_PARAM,
		FREQUENCY_CV13_PARAM,
		LEVEL14_PARAM,
		FREQUENCY14_PARAM,
		FREQUENCY_CV14_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		EF_RESPONSE_INPUT,
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
		LEVEL7_INPUT,
		FREQUENCY_CV7_INPUT,
		LEVEL8_INPUT,
		FREQUENCY_CV8_INPUT,
		LEVEL9_INPUT,
		FREQUENCY_CV9_INPUT,
		LEVEL10_INPUT,
		FREQUENCY_CV10_INPUT,
		LEVEL11_INPUT,
		FREQUENCY_CV11_INPUT,
		LEVEL12_INPUT,
		FREQUENCY_CV12_INPUT,
		LEVEL13_INPUT,
		FREQUENCY_CV13_INPUT,
		LEVEL14_INPUT,
		FREQUENCY_CV14_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		ODDS_OUTPUT,
		OUT_OUTPUT,
		EVENS_OUTPUT,
		EF1_OUTPUT,
		EF2_OUTPUT,
		EF3_OUTPUT,
		EF4_OUTPUT,
		EF5_OUTPUT,
		EF6_OUTPUT,
		EF7_OUTPUT,
		EF8_OUTPUT,
		EF9_OUTPUT,
		EF10_OUTPUT,
		EF11_OUTPUT,
		EF12_OUTPUT,
		EF13_OUTPUT,
		EF14_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		FMOD_RELATIVE_LIGHT,
		FMOD_FULL_LIGHT,
		NUM_LIGHTS
	};

	PEQEngine* _engines[maxChannels] {};
	float _rmsSums[14] {};
	float _rms[14] {};
	RootMeanSquare _efs[maxChannels][14];
	float _response = -1.0f;
	MultimodeFilter::Mode _lowMode = MultimodeFilter::LOWPASS_MODE;
	MultimodeFilter::Mode _highMode = MultimodeFilter::HIGHPASS_MODE;
	bool _fullFrequencyMode = false;

	PEQ14() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		float levelDefault = fabsf(PEQChannel::minDecibels) / (PEQChannel::maxDecibels - PEQChannel::minDecibels);
		configParam(EF_RESPONSE_PARAM, 0.0f, 1.0f, 0.3f, "Envelope follower response", "%", 0.0f, 100.0f);
		configParam(FREQUENCY_CV_PARAM, -1.0f, 1.0f, 0.0f, "Global frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(BANDWIDTH_PARAM, 0.0f, 1.0f, 0.11f, "Bandwidth", "%", 0.0f, 100.0f);
		configParam(LP_PARAM, 0.0f, 1.0f, 1.0f, "Channel 1 LP/BP");
		configParam(HP_PARAM, 0.0f, 1.0f, 1.0f, "Channel 14 HP/BP");
		configParam(FMOD_PARAM, 0.0f, 1.0f, 0.0f, "Frequency modulation range");
		configParam(LEVEL1_PARAM, 0.0f, 1.0f, levelDefault, "Channel 1 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY1_PARAM, 0.0f, 1.0f, 0.0689202f, "Channel 1 frequency", " HZ");
		configParam(FREQUENCY_CV1_PARAM, -1.0f, 1.0f, 1.0f, "Channel 1 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL2_PARAM, 0.0f, 1.0f, levelDefault, "Channel 2 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY2_PARAM, 0.0f, 1.0f, 0.0790569f, "Channel 2 frequency", " HZ");
		configParam(FREQUENCY_CV2_PARAM, -1.0f, 1.0f, 1.0f, "Channel 2 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL3_PARAM, 0.0f, 1.0f, levelDefault, "Channel 3 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY3_PARAM, 0.0f, 1.0f, 0.0935414f, "Channel 3 frequency", " HZ");
		configParam(FREQUENCY_CV3_PARAM, -1.0f, 1.0f, 1.0f, "Channel 3 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL4_PARAM, 0.0f, 1.0f, levelDefault, "Channel 4 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY4_PARAM, 0.0f, 1.0f, 0.1118034f, "Channel 4 frequency", " HZ");
		configParam(FREQUENCY_CV4_PARAM, -1.0f, 1.0f, 1.0f, "Channel 4 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL5_PARAM, 0.0f, 1.0f, levelDefault, "Channel 5 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY5_PARAM, 0.0f, 1.0f, 0.1322876f, "Channel 5 frequency", " HZ");
		configParam(FREQUENCY_CV5_PARAM, -1.0f, 1.0f, 1.0f, "Channel 5 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL6_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY6_PARAM, 0.0f, 1.0f, 0.1581139f, "Channel 6 frequency", " HZ");
		configParam(FREQUENCY_CV6_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL7_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY7_PARAM, 0.0f, 1.0f, 0.1870829f, "Channel 6 frequency", " HZ");
		configParam(FREQUENCY_CV7_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL8_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY8_PARAM, 0.0f, 1.0f, 0.2236068f, "Channel 6 frequency", " HZ");
		configParam(FREQUENCY_CV8_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL9_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY9_PARAM, 0.0f, 1.0f, 0.2645751f, "Channel 6 frequency", " HZ");
		configParam(FREQUENCY_CV9_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL10_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY10_PARAM, 0.0f, 1.0f, 0.3162278f, "Channel 6 frequency", " HZ");
		configParam(FREQUENCY_CV10_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL11_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY11_PARAM, 0.0f, 1.0f, 0.3741657f, "Channel 6 frequency", " HZ");
		configParam(FREQUENCY_CV11_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL12_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY12_PARAM, 0.0f, 1.0f, 0.4472136f, "Channel 6 frequency", " HZ");
		configParam(FREQUENCY_CV12_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL13_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY13_PARAM, 0.0f, 1.0f, 0.5291503f, "Channel 6 frequency", " HZ");
		configParam(FREQUENCY_CV13_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(LEVEL14_PARAM, 0.0f, 1.0f, levelDefault, "Channel 6 level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(FREQUENCY14_PARAM, 0.0f, 1.0f, 0.5873670f, "Channel 6 frequency", " HZ");
		configParam(FREQUENCY_CV14_PARAM, -1.0f, 1.0f, 1.0f, "Channel 6 frequency CV attenuation", "%", 0.0f, 100.0f);
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
