#pragma once

#include "bogaudio.hpp"
#include "filter.hpp"
#include "signal.hpp"

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
		BAND_1_LIGHT,
		BAND_5_LIGHT,
		BAND_9_LIGHT,
		BAND_2_LIGHT,
		BAND_6_LIGHT,
		BAND_10_LIGHT,
		BAND_3_LIGHT,
		BAND_7_LIGHT,
		BAND_11_LIGHT,
		BAND_4_LIGHT,
		BAND_8_LIGHT,
		BAND_12_LIGHT,
		LOWWPASS_LIGHT,
		HIGHPASS_LIGHT,
		NUM_LIGHTS
	};

	struct Engine {
		MultimodeFilter _filters[14];
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
		configParam<AmpliferParamQuantity>(BAND_1_PARAM, 0.0f, 1.0f, 1.0f, "Band 1 level");
		configParam<AmpliferParamQuantity>(BAND_5_PARAM, 0.0f, 1.0f, 1.0f, "Band 5 level");
		configParam<AmpliferParamQuantity>(BAND_9_PARAM, 0.0f, 1.0f, 1.0f, "Band 9 level");
		configParam<AmpliferParamQuantity>(BAND_2_PARAM, 0.0f, 1.0f, 1.0f, "Band 2 level");
		configParam<AmpliferParamQuantity>(BAND_6_PARAM, 0.0f, 1.0f, 1.0f, "Band 6 level");
		configParam<AmpliferParamQuantity>(BAND_10_PARAM, 0.0f, 1.0f, 1.0f, "Band 10 level");
		configParam<AmpliferParamQuantity>(BAND_3_PARAM, 0.0f, 1.0f, 1.0f, "Band 3 level");
		configParam<AmpliferParamQuantity>(BAND_7_PARAM, 0.0f, 1.0f, 1.0f, "Band 7 level");
		configParam<AmpliferParamQuantity>(BAND_11_PARAM, 0.0f, 1.0f, 1.0f, "Band 11 level");
		configParam<AmpliferParamQuantity>(BAND_4_PARAM, 0.0f, 1.0f, 1.0f, "Band 4 level");
		configParam<AmpliferParamQuantity>(BAND_8_PARAM, 0.0f, 1.0f, 1.0f, "Band 8 level");
		configParam<AmpliferParamQuantity>(BAND_12_PARAM, 0.0f, 1.0f, 1.0f, "Band 12 level");
		configParam<AmpliferParamQuantity>(LOWPASS_PARAM, 0.0f, 1.0f, 1.0f, "Lowpass level");
		configParam(CV_PARAM, 0.0f, 1.0f, 1.0f, "Level CV", "%", 0.0f, 100.0f);
		configParam<AmpliferParamQuantity>(HIGHPASS_PARAM, 0.0f, 1.0f, 1.0f, "Highpass level");
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
