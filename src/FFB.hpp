#pragma once

#include "bogaudio.hpp"
#include "filters/multimode.hpp"

using namespace bogaudio::dsp;

extern Model* modelFFB;

namespace bogaudio {

struct FFB : BGModule {
	enum ParamsIds {
		LOWPASS_PARAM,
		BAND_1_PARAM,
		BAND_2_PARAM,
		BAND_3_PARAM,
		BAND_4_PARAM,
		BAND_5_PARAM,
		BAND_6_PARAM,
		BAND_7_PARAM,
		BAND_8_PARAM,
		BAND_9_PARAM,
		BAND_10_PARAM,
		BAND_11_PARAM,
		BAND_12_PARAM,
		HIGHPASS_PARAM,
		CV_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		IN_INPUT,
		CV_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		ALL_OUTPUT,
		ODD_OUTPUT,
		EVEN_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		NUM_LIGHTS
	};

	struct Engine {
		MultimodeFilter _lowPass;
		FourPoleButtworthBandpassFilter _bandPasses[12];
		MultimodeFilter _highPass;
		Amplifier _amplifiers[14];
		bogaudio::dsp::SlewLimiter _slews[14];

		Engine() {
			sampleRateChange();
		}

		void sampleRateChange();
	};

	Engine* _engines[maxChannels] {};
	float _levels[14] {};

	FFB() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam<AmplifierParamQuantity>(BAND_1_PARAM, 0.0f, 1.0f, 1.0f, "Band 1 level");
		configParam<AmplifierParamQuantity>(BAND_5_PARAM, 0.0f, 1.0f, 1.0f, "Band 5 level");
		configParam<AmplifierParamQuantity>(BAND_9_PARAM, 0.0f, 1.0f, 1.0f, "Band 9 level");
		configParam<AmplifierParamQuantity>(BAND_2_PARAM, 0.0f, 1.0f, 1.0f, "Band 2 level");
		configParam<AmplifierParamQuantity>(BAND_6_PARAM, 0.0f, 1.0f, 1.0f, "Band 6 level");
		configParam<AmplifierParamQuantity>(BAND_10_PARAM, 0.0f, 1.0f, 1.0f, "Band 10 level");
		configParam<AmplifierParamQuantity>(BAND_3_PARAM, 0.0f, 1.0f, 1.0f, "Band 3 level");
		configParam<AmplifierParamQuantity>(BAND_7_PARAM, 0.0f, 1.0f, 1.0f, "Band 7 level");
		configParam<AmplifierParamQuantity>(BAND_11_PARAM, 0.0f, 1.0f, 1.0f, "Band 11 level");
		configParam<AmplifierParamQuantity>(BAND_4_PARAM, 0.0f, 1.0f, 1.0f, "Band 4 level");
		configParam<AmplifierParamQuantity>(BAND_8_PARAM, 0.0f, 1.0f, 1.0f, "Band 8 level");
		configParam<AmplifierParamQuantity>(BAND_12_PARAM, 0.0f, 1.0f, 1.0f, "Band 12 level");
		configParam<AmplifierParamQuantity>(LOWPASS_PARAM, 0.0f, 1.0f, 1.0f, "Lowpass level");
		configParam(CV_PARAM, 0.0f, 1.0f, 1.0f, "Level CV", "%", 0.0f, 100.0f);
		configParam<AmplifierParamQuantity>(HIGHPASS_PARAM, 0.0f, 1.0f, 1.0f, "Highpass level");
	}

	void sampleRateChange() override;
	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
