#pragma once

#include "bogaudio.hpp"
#include "dsp/signal.hpp"

extern Model* modelCmpDist;

using namespace bogaudio::dsp;

namespace bogaudio {

struct CmpDist : BGModule {
	enum ParamsIds {
		A_PARAM,
		WINDOW_PARAM,
		B_PARAM,
		GT_MIX_PARAM,
		EQ_MIX_PARAM,
		LT_MIX_PARAM,
		DRY_WET_PARAM,
		A_DRY_PARAM,
		B_DRY_PARAM,
		NUM_PARAMS
	};

	enum InputsIds {
		GT_MIX_INPUT,
		LT_MIX_INPUT,
		WINDOW_INPUT,
		DRY_WET_INPUT,
		A_INPUT,
		A_SCALE_INPUT,
		B_INPUT,
		B_SCALE_INPUT,
		NUM_INPUTS
	};

	enum OutputsIds {
		LT_MIX_OUTPUT,
		GT_OUTPUT,
		LT_OUTPUT,
		EQ_OUTPUT,
		MIX_OUTPUT,
		NUM_OUTPUTS
	};

	struct Engine {
		float aScale = 0.0f;
		float bScale = 0.0f;
		float window = 0.0f;
		float gtMix = 0.0f;
		float eqMix = 0.0f;
		float ltMix = 0.0f;
		CrossFader dryWet;
	};

	Engine* _engines[maxChannels] {};
	Amplifier _aDryAmplifier;
	Amplifier _bDryAmplifier;

	CmpDist() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS);
		configParam(A_PARAM, -1.0f, 1.0f, 1.0f, "A scaling", "%", 0.0f, 100.0f);
		configParam(WINDOW_PARAM, 0.0f, 1.0f, 0.25f, "Window", " V", 0.0f, 10.0f);
		configParam(B_PARAM, -1.0f, 1.0f, 0.0f, "B scaling", "%", 0.0f, 100.0f);
		configParam(GT_MIX_PARAM, -1.0f, 1.0f, 1.0f, "Greater-than mix", "%", 0.0f, 100.0f);
		configParam(EQ_MIX_PARAM, -1.0f, 1.0f, 0.0f, "Equal mix", "%", 0.0f, 100.0f);
		configParam(LT_MIX_PARAM, -1.0f, 1.0f, 1.0f, "Less-than mix", "%", 0.0f, 100.0f);
		configParam(DRY_WET_PARAM, -1.0f, 1.0f, 0.0f, "Dry/wet mix", "%", 0.0f, 100.0f);
		configParam<AmplifierParamQuantity>(A_DRY_PARAM, 0.0f, 1.0f, 1.0f, "A dry level");
		configParam<AmplifierParamQuantity>(B_DRY_PARAM, 0.0f, 1.0f, 1.0f, "B dry level");
	}

	bool active() override;
	int channels() override;
	void addChannel(int c) override;
	void removeChannel(int c) override;
	void modulate() override;
	void modulateChannel(int c) override;
	void processAlways(const ProcessArgs& args) override;
	void processChannel(const ProcessArgs& args, int c) override;
};

} // namespace bogaudio
