#pragma once

#include "bogaudio.hpp"
#include "parametric_eq.hpp"

extern Model* modelPEQ;

namespace bogaudio {

struct PEQ : BGModule {
	enum ParamsIds {
		A_LEVEL_PARAM,
		A_FREQUENCY_PARAM,
		A_BANDWIDTH_PARAM,
		A_CV_PARAM,
		B_LEVEL_PARAM,
		B_FREQUENCY_PARAM,
		B_BANDWIDTH_PARAM,
		B_CV_PARAM,
		C_LEVEL_PARAM,
		C_FREQUENCY_PARAM,
		C_BANDWIDTH_PARAM,
		C_CV_PARAM,
		A_MODE_PARAM,
		C_MODE_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		A_LEVEL_INPUT,
		B_LEVEL_INPUT,
		C_LEVEL_INPUT,
		A_FREQUENCY_INPUT,
		B_FREQUENCY_INPUT,
		C_FREQUENCY_INPUT,
		A_BANDWIDTH_INPUT,
		B_BANDWIDTH_INPUT,
		C_BANDWIDTH_INPUT,
		ALL_CV_INPUT,
		IN_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		OUT_OUTPUT,
		NUM_OUTPUTS
	};

	enum LightsIds {
		A_LIGHT,
		B_LIGHT,
		C_LIGHT,
		NUM_LIGHTS
	};

	PEQEngine* _engines[maxChannels] {};
	float _rmsSums[3] {};

	PEQ() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		float levelDefault = fabsf(PEQChannel::minDecibels) / (PEQChannel::maxDecibels - PEQChannel::minDecibels);
		configParam(A_LEVEL_PARAM, 0.0f, 1.0f, levelDefault, "Channel A level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(A_FREQUENCY_PARAM, 0.0f, 1.0f, 0.0707107f, "Channel A frequency", " HZ");
		configParam(A_BANDWIDTH_PARAM, 0.0f, 1.0f, 0.5f, "Channel A bandwidth", "%", 0.0f, 100.0f);
		configParam(A_CV_PARAM, -1.0f, 1.0f, 0.0f, "Channel A frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(A_MODE_PARAM, 0.0f, 1.0f, 1.0f, "Channel A LP/BP");
		configParam(B_LEVEL_PARAM, 0.0f, 1.0f, levelDefault, "Channel B level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(B_FREQUENCY_PARAM, 0.0f, 1.0f, 0.1322876f, "Channel B frequency", " HZ");
		configParam(B_BANDWIDTH_PARAM, 0.0f, 1.0f, 0.66f, "Channel B bandwidth", "%", 0.0f, 100.0f);
		configParam(B_CV_PARAM, -1.0f, 1.0f, 0.0f, "Channel B frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(C_LEVEL_PARAM, 0.0f, 1.0f, levelDefault, "Channel C level", " dB", 0.0f, PEQChannel::maxDecibels - PEQChannel::minDecibels, PEQChannel::minDecibels);
		configParam<ScaledSquaringParamQuantity<(int)PEQChannel::maxFrequency>>(C_FREQUENCY_PARAM, 0.0f, 1.0f, 0.223607f, "Channel C frequency", " HZ");
		configParam(C_BANDWIDTH_PARAM, 0.0f, 1.0f, 0.5f, "Channel C bandwidth", "%", 0.0f, 100.0f);
		configParam(C_CV_PARAM, -1.0f, 1.0f, 0.0f, "Channel C frequency CV attenuation", "%", 0.0f, 100.0f);
		configParam(C_MODE_PARAM, 0.0f, 1.0f, 1.0f, "Channel C HP/BP");
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
